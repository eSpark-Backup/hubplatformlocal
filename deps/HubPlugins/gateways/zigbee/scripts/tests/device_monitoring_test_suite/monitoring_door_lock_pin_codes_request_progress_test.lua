local tests = require( "tests" )
local core = require( "core" )
local storage = require( "storage" )

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
        },
        {
            name = "ProgrammingEventNotification"
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

local storage_helper = hubplugins.object( "storage_helper", "HUB:zigbee/scripts/utils" )
local item_id = storage_helper:call(
    "find_item_id",
    {
        item_name = "user_codes_scan_progress",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")

local function test_impl(progress)
    core.update_item_value = tests.create_mock()

    events_handler( {
        event = "user_codes_request_progress",
        node_id = 42,
        endpoint_id = 1,
        value = progress
    } )

    tests.assert_called(
        core.update_item_value,
        "check if pin code added",
        { { item_id, progress } }
    )
end

for i = 0, 100 do
    test_impl(i)
end
