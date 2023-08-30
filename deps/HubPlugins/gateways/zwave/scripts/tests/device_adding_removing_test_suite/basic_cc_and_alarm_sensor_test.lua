local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )

zwave.request_node_values = tests.create_mock()

local function test_impl( sensor_types, product_info )
    local cc_alarm_sensor = {
        id = class_ids.alarm_sensor,
        sensor_types = sensor_types
    }

    local cc_basic = {
        id = class_ids.basic,
    }

    local channel_0 = {
        id = 0,
        classes = { cc_alarm_sensor, cc_basic }
    }

    local node = {
        id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = false,

        specific_type = "",
        generic_type = "GENERIC_TYPE_SENSOR_ALARM",

        security = { "none" },
        security_class_started = { "none" },

        channels = { channel_0 }
    }

    -- prepare zwave
    zwave.add_node( node )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    -- check adding result
    local expected_heat_items = {
        {
            name = "heat_alarm",
            cc_id = class_ids.alarm_sensor,
            channel_id = 0,
            tag = "heat_alarm",
        },
        {
            name = "security_threat",
            tag = "heat_alarm.security_tag",
            cc_id = class_ids.alarm_sensor,
            channel_id = 0,
            main_item_name = "heat_alarm"
        }
    }

    local expected_smoke_items = {
        {
            name = "smoke_alarm",
            cc_id = class_ids.alarm_sensor,
            tag = "smoke_alarm",
            channel_id = 0,
        },
        {
            name = "basic",
            cc_id = class_ids.basic,
            channel_id = 0,
        },
        {
            name = "security_threat",
            tag = "smoke_alarm.security_tag",
            cc_id = class_ids.alarm_sensor,
            channel_id = 0,
            main_item_name = "smoke_alarm"
        }
    }


    local expected_devices = {
        {
            name = "Smoke Sensor",
            category = "security_sensor",
            subcategory = "smoke",
            items = expected_smoke_items,
            node_id = "9000",
            battery_powered = false,

            subdevices = {
                {
                    name = "Heat Sensor",
                    category = "security_sensor",
                    subcategory = "heat",
                    items = expected_heat_items
                }
            }
        }
    }

    local check_context = {
        descriptors = {},
        item_ids = {},
        device_ids = {}
    }

    local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

    -- send smoke basic event
    local function find_item( item_name, item_tag )
        return core.get_item( storage:call("find_item_id", item_name, 9000, class_ids.alarm_sensor, 0, item_tag  ) )
    end

    local smoke_item = function() return find_item( "smoke_alarm", "smoke_alarm" ) end
    local security_smoke_item = function() return find_item( "smoke_alarm", "smoke_alarm.security_tag" ) end
    local heat_item = function() return find_item( "heat_alarm", "heat_alarm" ) end
    local security_heat_item = function() return find_item( "heat_alarm", "heat_alarm.security_tag" ) end

    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.basic,
        value = 0xFF
    } )

    tests.assert_eq( smoke_item().value, "smoke_detected", "check smoke value after basic value changing" )
    tests.assert_eq( security_smoke_item().value, true, "check smoke security value after basic value changing" )
    tests.assert_eq( heat_item().value, "heat_ok", "check heat value after basic value changing" )
    tests.assert_eq( security_heat_item().value, false, "check heat security value after basic value changing" )

    -- send smoke alarm sensor event
    events_handler( {
        event = "alarm_sensor_value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.alarm_sensor,
        sensor_type = "smoke_alarm",
        value = 0
    } )

    tests.assert_eq( smoke_item().value, "no_smoke", "check smoke value after alarm sensor value changing" )
    tests.assert_eq( security_smoke_item().value, false, "check smoke security value after alarm sensor value changing" )
    tests.assert_eq( heat_item().value, "heat_ok", "check heat value after alarm sensor value changing" )
    tests.assert_eq( security_heat_item().value, false, "check heat security value after alarm sensor value changing" )

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )

    -- check node removing result
    hubplugins.weak_loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
end


tests.run_with_params( test_impl,
{
    {
        value = {
            "heat_alarm",
        },
        description = "smoke is absent in alarm sensor types"
    },
    {
        value = {
            "heat_alarm",
            "smoke_alarm",
        },
        description = "smoke is present in alarm sensor types"
    }
},
{
    {
        value = {
            manufacturer_id = 271,
            product_type = 3074,
            product_id = 4099,
        },
        description = "271_3074_4099"
    },
    {
        value = {
            manufacturer_id = 316,
            product_type = 2,
            product_id = 30,
        },
        description = "316_2_30"
    }
} )

tests.run_with_params( test_impl,
{
    {
        value = {
            "heat_alarm",
            "smoke_alarm",
        },
        description = "smoke is present in alarm sensor types"
    }
},
{
    {
        value = {
            manufacturer_id = 1,
            product_type = 2,
            product_id = 3,
        },
        description = "1_2_3"
    }
} )
