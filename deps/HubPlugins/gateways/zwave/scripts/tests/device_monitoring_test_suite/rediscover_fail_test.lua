local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require("core")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require("HUB:zwave/scripts/resources/item_names")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()
core.send_ui_broadcast = tests.create_mock()

local cc_binary_sensor = { 
    id = class_ids.sensor_binary,
    sensor_types = { 0x03, 0x04 }
}

local channel_0 = {
    id = 0,
    classes = { cc_binary_sensor }
}

local node = {
    id = "9000",
    manufacturer_id = 0,
    product_type = 1,
    product_id = 2,
    is_battery_powered = true,

    specific_type = "",
    generic_type = "",

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

local device_id = storage:call("find_device_id", 9000)
local setting_id = storage:call("find_setting_id", 9000, nil, "rediscover")

core.update_setting_status(setting_id, "synced")

do
    events_handler( {
        event = "rediscover_device_progress",
        node_id = 9000,
        status = "started",
        progress = 0
    } )

    tests.assert_called(core.send_ui_broadcast, "check rediscover progress (started) sent", { {
            {
                event = "rediscover_device_progress", 
                progress = 0, 
                status = "started",
                device_id = device_id
            }
        } } )

    tests.assert_eq(core.get_device(device_id).status, "rediscovering")
    tests.assert_eq(core.get_setting(setting_id).status, "pending")
end

cc_binary_sensor.sensor_types = { 0x03, 0x05 }
zwave.add_node( node )

do
    events_handler( {
        event = "rediscover_device_progress",
        node_id = 9000,
        status = "failed",
        progress = 100
    } )

    tests.assert_called( core.send_ui_broadcast, "check rediscover progress (fail) sent", { {
            {
                event = "rediscover_device_progress", 
                progress = 100, 
                status = "failed",
                device_id = device_id
            }
        } } )

    tests.assert_eq(core.get_device(device_id).status, "idle")
    tests.assert_eq(core.get_setting(setting_id).status, "failed")
end


local expected_devices = {
    {
        name = "CO Sensor",
        category = "security_sensor",
        subcategory = "co",
        items = {
            {
                name = "co_alarm",
                tag = "co_alarm",
                cc_id = class_ids.sensor_binary,
                channel_id = 0,
                value_type = "token",
                enum = {
                    "unknown",
                    "no_co",
                    "co_detected",
                }
            },
            {
                name = "security_threat",
                tag = "co_alarm.security_tag",
                cc_id = class_ids.sensor_binary,
                channel_id = 0,
                main_item_name = "co_alarm"
            }
        },
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            }
        },
        node_id = "9000",
        battery_powered = true,

        subdevices = {
            {
                name = "CO2 Sensor",
                category = "security_sensor",
                subcategory = "co2",
                items = {
                    {
                        name = "co2_alarm",
                        tag = "co2_alarm",
                        cc_id = class_ids.sensor_binary,
                        channel_id = 0,
                        value_type = "token",
                        enum = {
                            "unknown",
                            "no_co2",
                            "co2_detected",
                        }
                    },
                    {
                        name = "security_threat",
                        tag = "co2_alarm.security_tag",
                        cc_id = class_ids.sensor_binary,
                        channel_id = 0,
                        main_item_name = "co2_alarm",
                    }
                }
            }
        }
    }
}


loadfile( "HUB:zwave/scripts/tests/device_adding_removing_test_suite/check_zwave_node_adding_result" )( expected_devices )
