local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )
local emergency_state = loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

house_modes.get_current = function() return "1" end

local cluster_ias_ace = {
    name = "ias_ace",
    data_holder = "client",
    attributes = {},
    commands = {
        {
            name = "Arm"
        },
        {
            name = "Emergency"
        },
        {
            name = "Panic"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_ias_ace }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Keypad",
    security = 0,
    is_reachable = 1,
    is_battery_powered = true,

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
        item_name = "emergency_state",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")

local security_item_id = storage:call(
    "find_item_id",
    {
        item_name = "emergency_state",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = "",
        tag = ".security_tag"
    })

tests.assert_ne(security_item_id, nil, "Try to find security item id")


-- test panic
core.update_item_value = tests.create_mock()

events_handler( {
    event = "simple_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_ace",
    command_name = "Panic"
} )

tests.assert_called(
    core.update_item_value,
    "check if item value updated",
    {
        { item_id, emergency_state.panic },
        { security_item_id, true },
        { item_id, emergency_state.idle },
        { security_item_id, false }
    }
)

-- test emergency
core.update_item_value = tests.create_mock()

events_handler( {
    event = "simple_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_ace",
    command_name = "Emergency"
} )

tests.assert_called(
    core.update_item_value,
    "check if item value updated",
    {
        { item_id, emergency_state.emergency },
        { security_item_id, true },
        { item_id, emergency_state.idle },
        { security_item_id, false }
    }
)
