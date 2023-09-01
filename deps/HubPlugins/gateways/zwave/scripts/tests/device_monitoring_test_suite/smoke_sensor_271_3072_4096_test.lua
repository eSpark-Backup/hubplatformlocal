local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()


local cc_alarm_sensor = {
    id = class_ids.alarm_sensor,
    sensor_types = {
        "smoke_alarm", -- Smoke Alarm
        "heat_alarm"  -- Heat Alarm
    }
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
    manufacturer_id = 271,
    product_type = 3072,
    product_id = 4096,
    is_battery_powered = true,

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

-- send alarm sensor event
events_handler( {
    event = "value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.basic,
    value = 0xFF,
} )

-- send alarm basic event
local function find_item( item_name, item_tag )
    return core.get_item( storage:call("find_item_id", item_name, 9000, class_ids.alarm_sensor, 0, item_tag  ) )
end

local smoke_alarm_item = find_item( "smoke_alarm", "smoke_alarm" )
local security_threat_item = find_item( "smoke_alarm", "smoke_alarm.security_tag" )

tests.assert_eq( smoke_alarm_item.value, "smoke_detected", "check alarm value after alarm sensor value changing" )
tests.assert_eq( security_threat_item.value, true, "check alarm security value after alarm sensor value changing" )
