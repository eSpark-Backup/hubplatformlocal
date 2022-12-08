local tests = require( "tests" )
local core = require( "core" )

local cluster_door_lock = {
    name = "door_lock",
    attributes = {
        {
            name = "LockState",
            formatted_value = "locked"
        }
    },
    commands = {
        {
            name = "SetPinCode"
        },
        {
            name = "GetPinCode"
        },
        {
            name = "ClearPinCode"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_door_lock, }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Door Lock",
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
        item_name = "door_lock",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = "LockState"
    })

tests.assert_ne(item_id, nil, "Try to find item id")

local function check_item_set(item_value, command_params)
    zigbee.send_command = tests.create_mock()

    loadfile( "HUB:zigbee/scripts/set_item_value" )(
        {
            item_id = item_id,
            value = item_value
        }
    )

    tests.assert_called(
        zigbee.send_command,
        "check if panel status changed sent",
        command_params
    )
end

-- single state in attr
check_item_set( "unknown", {} )
check_item_set( "unsecured", {{ 42, 1, "door_lock", "UnlockDoor" }} )
check_item_set( "secured", {{ 42, 1, "door_lock", "LockDoor" }} )
