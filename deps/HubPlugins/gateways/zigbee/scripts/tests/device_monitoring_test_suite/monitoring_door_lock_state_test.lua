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

local function check_item_updated(attr_value, new_item_value)
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "attribute_value_updated",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute = {
            name = "LockState",
            formatted_value = attr_value
        }
    } )

    tests.assert_called( core.update_item_value, "check if item values updated", {{ item_id, new_item_value }} )
end

-- single state in attr
check_item_updated( "unknown", "unknown" )
check_item_updated( "not_fully_locked", "unsecured" )
check_item_updated( "unlocked", "unsecured" )
check_item_updated( "locked", "secured" )
