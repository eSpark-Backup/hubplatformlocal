local tests = require( "tests" )
local zwave = require("zwave")
local core = require("core")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local item_names = require( "HUB:zwave/scripts/resources/item_names" )
local battery_level = loadfile("HUB:zwave/scripts/resources/battery_level")()

zwave.request_node_values = tests.create_mock()

local channel_0 = {
    id = 0,
    classes = { { id = class_ids.battery }, }
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

local function send_battery_value(value)
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.battery,
        channel_id = 0,
        value = value
    } )
end

local function get_battery_item_value()
    return core.get_item( storage:call("find_item_id", item_names.battery, 9000--[[node_id]], class_ids.battery, 0--[[channel_id]] ) ).value
end

local function send_and_check_battery_value(send_value, expected_value)
    send_battery_value(send_value)
    tests.assert_eq( get_battery_item_value(), expected_value )
end

send_and_check_battery_value( 0, 0 )
send_and_check_battery_value( 45, 45 )
send_and_check_battery_value( 100, 100 )
send_and_check_battery_value( 0xFF, battery_level.low )
