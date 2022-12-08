local tests = require( "tests" )
local core = require( "core" )
local zigbee = require( "zigbee" )
local timer = require( "timer" )

timer.set_timeout = tests.create_mock()

local cluster_level_control = {
    name = "level_control",
    attributes = {
        {
            name = "CurrentLevel",
            formatted_value = 0
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_level_control }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Bulb",
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
        item_name = "dimmer",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel"
    })

tests.assert_ne(item_id, nil, "Try to find dimmer item id")

-- check dimmer 1
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = 1} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "level_control", "MoveToLevelWithOnOff", 3 } } )

-- check dimmer 10
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = 10} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "level_control", "MoveToLevelWithOnOff", 25 } } )

-- check dimmer 100
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = 100} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "level_control", "MoveToLevelWithOnOff", 254 } } )

-- check dimmer_up
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "dimmer_up",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel"
    })

tests.assert_ne(item_id, nil, "Try to find dimmer_up item id")

zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = 1} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "level_control", "MoveWithOnOff", "up" } } )

-- check dimmer_down
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "dimmer_down",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel"
    })

tests.assert_ne(item_id, nil, "Try to find dimmer_down item id")

zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = 1} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "level_control", "MoveWithOnOff", "down" } } )

-- check dimmer_stop
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "dimmer_stop",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "level_control",
        attribute_name = "CurrentLevel"
    })

tests.assert_ne(item_id, nil, "Try to find dimmer_stop item id")

zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_value" )( {item_id = item_id, value = 1} )

tests.assert_called( zigbee.send_command, "check if on command sent", { { 42, 1, "level_control", "StopWithOnOff" } } )
