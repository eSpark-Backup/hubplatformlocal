local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local function test_impl( sensor_types, product_info )
    local cc_binary_sensor = {
        id = class_ids.sensor_binary,
        sensor_types = sensor_types
    }

    local cc_basic = {
        id = class_ids.basic,
    }

    local channel_0 = {
        id = 0,
        classes = { cc_binary_sensor, cc_basic }
    }

    local node = {
        id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id,
        is_battery_powered = false,

        specific_type = "",
        generic_type = "GENERIC_TYPE_SENSOR_BINARY",

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
    local expected_glass_items = {
        {
            name = "glass_breakage_alarm",
            cc_id = class_ids.sensor_binary,
            channel_id = 0,
            tag = "home_security",
        },
        {
            name = "security_threat",
            tag = "home_security.security_tag",
            cc_id = class_ids.sensor_binary,
            channel_id = 0,
            main_item_name = "glass_breakage_alarm"
        }
    }

    local expected_motion_items = {
        {
            name = "motion",
            cc_id = class_ids.sensor_binary,
            tag = "home_security",
            channel_id = 0,
        },
        {
            name = "basic",
            cc_id = class_ids.basic,
            channel_id = 0,
        },
        {
            name = "security_threat",
            tag = "home_security.security_tag",
            cc_id = class_ids.sensor_binary,
            channel_id = 0,
            main_item_name = "motion"
        }
    }


    local expected_devices = {
        {
            name = "Motion Sensor",
            category = "security_sensor",
            subcategory = "motion",
            items = expected_motion_items,
            node_id = "9000",
            battery_powered = false,

            subdevices = {
                {
                    name = "Glass Break Sensor",
                    category = "security_sensor",
                    subcategory = "glass",
                    items = expected_glass_items
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

    -- send motion basic event
    local function find_item( item_name, item_tag )
        return core.get_item( storage:call("find_item_id", item_name, 9000, class_ids.sensor_binary, 0, item_tag  ) )
    end

    local motion_item = function() return find_item( "motion", "home_security" ) end
    local security_motion_item = function() return find_item( "motion", "home_security.security_tag" ) end
    local glass_item = function() return find_item( "glass_breakage_alarm", "home_security" ) end
    local security_glass_item = function() return find_item( "glass_breakage_alarm", "home_security.security_tag" ) end

    events_handler( {
        event = "value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.basic,
        value = 0xFF
    } )

    tests.assert_eq( motion_item().value, true, "check motion value after basic value changing" )
    tests.assert_eq( security_motion_item().value, true, "check motion security value after basic value changing" )
    tests.assert_eq( glass_item().value, "unknown", "check glass value after basic value changing" )
    tests.assert_eq( security_glass_item().value, false, "check glass security value after basic value changing" )

    -- send motion binary sensor event
    events_handler( {
        event = "binary_sensor_value_updated",
        node_id = "9000",
        channel_id = 0,
        class_id = class_ids.sensor_binary,
        sensor_type = 0x0C,
        value = 0
    } )

    tests.assert_eq( motion_item().value, false, "check motion value after binary sensor value changing" )
    tests.assert_eq( security_motion_item().value, false, "check motion security value after binary sensor value changing" )
    tests.assert_eq( glass_item().value, "unknown", "check glass value after binary sensor value changing" )
    tests.assert_eq( security_glass_item().value, false, "check glass security value after binary sensor value changing" )

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
            0x0D, -- glass
        },
        description = "motion is absent in binary sensor types"
    },
    {
        value = {
            0x0D, -- glass
            0x0C, -- motion
        },
        description = "motion is present in binary sensor types"
    }
},
{
    {
        value = {
            manufacturer_id = 5,
            product_type = 17217,
            product_id = 36864,
        },
        description = "5_17217_36864"
    },
    {
        value = {
            manufacturer_id = 265,
            product_type = 8194,
            product_id = 515,
        },
        description = "265_8194_515"
    }
} )

tests.run_with_params( test_impl,
{
    {
        value = {
            0x0D, -- glass
            0x0C, -- motion
        },
        description = "motion is present in binary sensor types"
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