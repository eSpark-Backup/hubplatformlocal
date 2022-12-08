local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require( "zwave" )
local core = require( "core" )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local node = {
    id = "9000",
    channels = {
        {
            classes = {
                { id = 94,  version = 2 },
                { id = 133, version = 2 },
                { id = 142, version = 3 },
                { id = 89,  version = 1 },
                { id = 134, version = 3 },
                { id = 114, version = 2 },
                { id = 112, version = 1 },
                { id = 132, version = 2 },
                { id = 128, version = 1 },
                { id = 48, sensor_types = { 10, 11 }, version = 2 },
                {
                    id = 113,
                    notifications = {
                        {
                            event_types = { "dw_is_opened","dw_is_closed" },
                            type = "access_control"
                        },
                        {
                            event_types = { "product_cover_removed" },
                            type = "home_security"
                        }
                    },
                    v1_supported = false,
                    version = 8
                }
            },
            id = 0
        }
    },
    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

    is_battery_powered = true,

    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,

    security = { "none" },
    security_class_started = { "unauthenticated" },
}

-- prepare zwave
zwave.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 9000
} )


-- check adding result
local expected_devices = {
    {
        name = "Door/Window Sensor",
        category = "security_sensor",
        subcategory = "door",
        items = {
            {
                name = "tampering_cover_alarm",
                cc_id = class_ids.notification,
                tag = "home_security",
                channel_id = 0,
            },
            {
                name = "battery",
                cc_id = class_ids.battery,
                channel_id = 0,
            },
            {
                name = "dw_state",
                cc_id = class_ids.notification,
                tag = "access_control",
                channel_id = 0,
                is_merged = true,
            },
            {
                name = "dw_state",
                cc_id = class_ids.sensor_binary,
                tag = "access_control",
                channel_id = 0,
            },
            {
                name = "security_threat",
                tag = "access_control.security_tag",
                cc_id = class_ids.sensor_binary,
                channel_id = 0,
                main_item_name = "dw_state"
            },
        },
        settings = {
            {
                value_type = "action",
                value = {
                    text = "Rediscover",
                    lang_tag = "rediscover_tag"
                },
                tag = "rediscover"
            },
            {
                value_type = "int",
                value = 5400,
                value_default = 5400,
                cc_id = class_ids.wake_up,
                tag = "wake_up_interval"
            },
            {
                cc_id = class_ids.configuration,
                value_type = "dictionary.zwave_configuration",
                value = {}
            }
        },
        node_id = "9000",
        battery_powered = true,
        security = "authenticated",

        subdevices = {
            {
                name = "Door/Window Tilt Sensor",
                category = "security_sensor",
                subcategory = "tilt",
                items = {
                    {
                        name = "tilt",
                        cc_id = class_ids.sensor_binary,
                        tag = "access_control",
                        channel_id = 0,
                        value_type = "token",
                        enum = { "unknown", "tilt", "no_tilt" },
                        value = "unknown"
                    },
                    {
                        name = "security_threat",
                        tag = "access_control.security_tag",
                        cc_id = class_ids.sensor_binary,
                        channel_id = 0,
                        main_item_name = "tilt"
                    },
                },
                settings = {
                    {
                        value_type = "bool",
                        value = true,
                        value_default = true,
                        cc_id = class_ids.sensor_binary,
                        tag = "tilt_alarm"
                    },
                }
            }
        }
    }
}

local check_context = {}

local test_suite_dir = "HUB:zwave/scripts/tests/device_adding_removing_test_suite/"

loadfile( test_suite_dir .. "check_zwave_node_adding_result" )( expected_devices, check_context )

-- remove node
events_handler( {
    event = "node_removed",
    node_id = "9000",
    manufacturer_id = 1,
    product_type = 2,
    product_id = 3,
} )

-- check node removing result
loadfile( test_suite_dir .. "check_zwave_node_removing_result" )( check_context )
