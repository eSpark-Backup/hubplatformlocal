local tests = require( "tests" )

local cluster_poll_control = {
    name = "poll_control",
    attributes = {}
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_poll_control }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Device",
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
local device_id = storage:call(
    "find_device_id",
    42)

tests.assert_ne(device_id, nil, "Try to find device id")

-- check stop fast poll
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/stop_fast_poll" )( {device_id = device_id} )


tests.assert_called(
    zigbee.send_command,
    "check if command sent",
    {
        { 42, 1, "poll_control", "FastPollStop" }
    }
)

-- check no stop fast poll
node.endpoints[1].clusters = {}

zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/stop_fast_poll" )( {device_id = device_id} )


tests.assert_called(
    zigbee.send_command,
    "check if no command sent",
    {}
)

