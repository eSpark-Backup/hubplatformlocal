local tests = require( "tests" )
local zwave = require( "zwave" )
local core = require("core")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local item_names = require("HUB:zwave/scripts/resources/item_names")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local cc_binary_sensor = { 
    id = class_ids.sensor_binary,
    sensor_types = { 0x03, 0x05 }
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

events_handler( {
    event = "rediscover_device_progress",
    node_id = 9000,
    status = "finished",
    progress = 100
} )

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
                name = "Heat Sensor",
                category = "security_sensor",
                subcategory = "heat",
                items = {
                    {
                        name = "heat_alarm",
                        tag = "heat_alarm",
                        cc_id = class_ids.sensor_binary,
                        channel_id = 0,
                        value_type = "token",
                        enum = {
                            "unknown",
                            "heat_ok",
                            "overheat_detected",
                        }
                    },
                    {
                        name = "security_threat",
                        tag = "heat_alarm.security_tag",
                        cc_id = class_ids.sensor_binary,
                        channel_id = 0,
                        main_item_name = "heat_alarm",
                    }
                }
            }
        }
    }
}


loadfile( "HUB:zwave/scripts/tests/device_adding_removing_test_suite/check_zwave_node_adding_result" )( expected_devices )
