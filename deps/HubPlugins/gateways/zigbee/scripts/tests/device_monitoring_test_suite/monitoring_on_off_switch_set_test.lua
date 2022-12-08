local tests = require( "tests" )
local core = require( "core" )
local zigbee = require( "zigbee" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()

local cluster_on_off = {
    name = "on_off",
    attributes = {
        {
            name = "OnOff",
            formatted_value = "false"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_on_off }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Plug",
    security = 0,
    is_reachable = 1,
    is_battery_powered = false,

    endpoints = { endpoint_1 }
}

local events_handler = assert( loadfile( "HUB:zigbee/scripts/events/zigbee/events_handling" ) )

local zigbee = require( "zigbee" )

-- prepare zigbee
zigbee.add_node( node )

-- add node
events_handler( {
    event = "node_added",
    node_id = 42
} )

local storage = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "switch",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "on_off",
        attribute_name = "OnOff"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- check enable switch
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = true} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "on_off", "On" } } )

-- check disable switch
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = false} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "on_off", "Off" } } )
