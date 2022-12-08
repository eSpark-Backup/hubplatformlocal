local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )

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

local tilt_item_id = storage:call("find_item_id", "tilt", 9000, class_ids.sensor_binary, 0, "access_control" )
local security_item_id = storage:call("find_item_id", "tilt", 9000, class_ids.sensor_binary, 0, "access_control.security_tag" )
local tilt_setting_id = storage:call("find_setting_id", 9000, class_ids.sensor_binary, "tilt_alarm" )

tests.assert_ne(tilt_item_id, nil)
tests.assert_ne(security_item_id, nil)
tests.assert_ne(tilt_setting_id, nil)

-- test tilted
core.update_item_value = tests.create_mock()

events_handler( {
    event = "binary_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.sensor_binary,
    sensor_type = 11,
    value = 0xFF,
} )

tests.assert_called(
    core.update_item_value,
    "check items updated",
    { { tilt_item_id, "tilt" }, { security_item_id, true } }
)

-- test no tilt
core.update_item_value = tests.create_mock()

events_handler( {
    event = "binary_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.sensor_binary,
    sensor_type = 11,
    value = 0x00,
} )

tests.assert_called(
    core.update_item_value,
    "check items updated",
    { { tilt_item_id, "no_tilt" }, { security_item_id, false } }
)

-- test tilt alarm disabled
core.set_setting_value(tilt_setting_id, false)

core.update_item_value = tests.create_mock()

events_handler( {
    event = "binary_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.sensor_binary,
    sensor_type = 11,
    value = 0xFF,
} )

tests.assert_called(
    core.update_item_value,
    "check items updated",
    { { tilt_item_id, "tilt" }, { security_item_id, false } }
)

-- test no tilt, alarms disabled
core.update_item_value = tests.create_mock()

events_handler( {
    event = "binary_sensor_value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.sensor_binary,
    sensor_type = 11,
    value = 0x00,
} )

tests.assert_called(
    core.update_item_value,
    "check items updated",
    { { tilt_item_id, "no_tilt" }, { security_item_id, false } }
)
