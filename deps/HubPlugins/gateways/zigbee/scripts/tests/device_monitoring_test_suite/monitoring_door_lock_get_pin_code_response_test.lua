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
        item_name = "user_codes",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")

-- test enabled
core.update_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "get_user_response_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    user_id = 1,
    pin_code = "1111",
    user_status = "occupied_enabled"
} )

tests.assert_called(
    core.update_item_dictionary_value,
    "check if pin code added",
    { { item_id, "1", { code = "1111", name = "1", mode = "enabled" } } }
)

-- test disabled
core.update_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "get_user_response_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    user_id = 1,
    pin_code = "1111",
    user_status = "occupied_disabled"
} )

tests.assert_called(
    core.update_item_dictionary_value,
    "check if pin code added",
    { { item_id, "1", { code = "1111", name = "1", mode = "disabled" } } }
)

-- test available
core.remove_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "get_user_response_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    user_id = 1,
    pin_code = "1111",
    user_status = "available"
} )

tests.assert_called(
    core.remove_item_dictionary_value,
    "check if pin code removed",
    { { item_id, "1" } }
)

-- test not_supported
core.remove_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "get_user_response_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    user_id = 1,
    pin_code = "1111",
    user_status = "not_supported"
} )

tests.assert_called(
    core.remove_item_dictionary_value,
    "check if pin code removed",
    { { item_id, "1" } }
)
