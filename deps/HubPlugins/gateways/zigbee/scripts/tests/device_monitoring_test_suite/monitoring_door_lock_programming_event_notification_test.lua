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

-- test remove pin code
core.remove_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_deleted",
    user_id = 1,
    pin_code = ""
} )

tests.assert_called(
    core.remove_item_dictionary_value,
    "check if pin code deleted",
    { { item_id, "1" } }
)

-- test pin code added
core.update_item_dictionary_value = tests.create_mock()
zigbee.request_user_codes = tests.create_mock()

events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_added",
    user_id = 1,
    pin_code = "1111",
    user_status = "occupied_enabled"
} )

tests.assert_called(
    core.update_item_dictionary_value,
    "check if pin code added",
    { { item_id, "1", { code = "1111", name = "1", mode = "enabled" } } }
)

events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_added",
    user_id = 1,
    pin_code = "",
    user_status = "occupied_enabled"
} )

tests.assert_called(
    zigbee.request_user_codes,
    "check if pin code value requested",
    {{ node.id, 1, 1, 1 } }
)

-- test pin code changed
core.update_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_changed",
    user_id = 2,
    pin_code = "2222",
    user_status = "occupied_enabled"
} )

tests.assert_called(
    core.update_item_dictionary_value,
    "check if pin code added",
    { { item_id, "2", { code = "2222", name = "2", mode = "enabled" } } }
)

-- test pin code added with saved name
storage.set_table( "usernames." .. item_id, {["3"] = "test_name3"} )
core.update_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_added",
    user_id = 3,
    pin_code = "3333",
    user_status = "occupied_enabled"
} )

tests.assert_called(
    core.update_item_dictionary_value,
    "check if pin code added",
    { { item_id, "3", { code = "3333", name = "test_name3", mode = "enabled" } } }
)

tests.assert_eq( storage.get_table("usernames." .. item_id), nil, "Check if user name deleted from storage" )

-- test pin code added disabled
core.update_item_dictionary_value = tests.create_mock()

events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_added",
    user_id = 4,
    pin_code = "4444",
    user_status = "occupied_disabled"
} )

tests.assert_called(
    core.update_item_dictionary_value,
    "check if pin code added",
    { { item_id, "4", { code = "4444", name = "4", mode = "disabled" } } }
)
