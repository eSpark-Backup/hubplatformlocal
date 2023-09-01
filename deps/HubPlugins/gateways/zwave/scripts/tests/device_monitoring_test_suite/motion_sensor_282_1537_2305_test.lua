local tests = require( "tests" )

local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

zwave.request_node_values = tests.create_mock()


local cc_binary_sensor = {
    id = class_ids.sensor_binary,
    sensor_types = {
        0x01, -- General purpose
    }
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
    manufacturer_id = 282,
    product_type = 1537,
    product_id = 2305,
    is_battery_powered = true,

    specific_type = "SPECIFIC_TYPE_ROUTING_SENSOR_BINARY",
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

-- send motion binary sensor event
events_handler( {
    event = "value_updated",
    node_id = "9000",
    channel_id = 0,
    class_id = class_ids.basic,
    value = 0xFF,
} )

-- send motion basic event
local function find_item( item_name, item_tag )
    return core.get_item( storage:call("find_item_id", item_name, 9000, class_ids.sensor_binary, 0, item_tag  ) )
end

local motion_item = find_item( "motion", "access_control" )
local security_motion_item = find_item( "motion", "access_control.security_tag" )

tests.assert_eq( motion_item.value, true, "check motion value after binary sensor value changing" )
tests.assert_eq( security_motion_item.value, true, "check motion security value after binary sensor value changing" )
