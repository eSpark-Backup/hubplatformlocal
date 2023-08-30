local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local tests = require("tests")
local events_handler = assert( hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/base_fixture")()
local zwave = require("zwave")
local core = require("core")

zwave.request_node_values = tests.create_mock()

local binary_sensor_command_class = {
    id = class_ids.sensor_binary,
    sensor_types = {
        6,
        8
    }
}

local multilevel_sensor_command_class = {
    id = class_ids.multilevel_sensor,
    sensors = {
        {
            scales = {
                "celsius",
                "fahrenheit"
            },
            sensor_type = "air_temperature"
        },
        {
            scales = {
                "percent"
            },
            sensor_type = "humidity"
        }
    }
}

local notification_command_class = {
    id = class_ids.notification,
    notifications = {
        {
            event_types = {
                "state_idle",
                "water_leak_detected"
            },
            type = "water_alarm",
            unknown_event_types =  {}
        },
        {
            event_types = {
                "product_cover_removed",
            },
            type = "home_security",
            unknown_event_types = {}
        }
    },
    v1_supported = false,
    version = 4
}

local channel_0 = {
    id = 0,
    classes = {
        notification_command_class,
        binary_sensor_command_class,
        multilevel_sensor_command_class
    }
}

local node = {
    id = "9000",
    manufacturer_id = 316,
    product_type = 2,
    product_id = 31,
    is_battery_powered = true,

    generic_type = "GENERIC_TYPE_SENSOR_NOTIFICATION",
    specific_type = "SPECIFIC_TYPE_NOTIFICATION_SENSOR",

    security = { "s0" },
    security_class_started = { "s0" },

    channels = { channel_0 }
}

fixture:add_node(node)

local tamper_item_id = storage:call("find_item_id", item_names.tampering_cover_alarm, 9000, class_ids.sensor_binary, 0, "home_security")
assert( tamper_item_id, "Unable to find tamper item id" )

-- Check update on notification
core.update_item_value = tests.create_mock()

events_handler( {
    event = "notification_recieved",
    node_id = 9000,
    class_id = class_ids.notification,
    channel_id = 0,
    v1_alarm_type = 0,
    v1_alarm_level = 0,
    event_type = "product_cover_removed",
    value = 0xFF,
    notification_type = "home_security"
} )

tests.assert_called( core.update_item_value, "check update on notification", { {tamper_item_id, "tampering_cover"} } )

-- test binary sensor tampered
core.update_item_value = tests.create_mock()

events_handler( {
    event = "binary_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.sensor_binary,
    sensor_type = 0x08,
    value = 0xFF,
} )

tests.assert_called( core.update_item_value, "check update on binary sensor", { {tamper_item_id, "tampering_cover"} } )

-- test binary sensor not tampered
core.update_item_value = tests.create_mock()

events_handler( {
    event = "binary_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.sensor_binary,
    sensor_type = 0x08,
    value = 0x00,
} )

tests.assert_called( core.update_item_value, "check update on binary sensor", { {tamper_item_id, "no_tampering_cover"} } )

fixture:remove_node(node)
