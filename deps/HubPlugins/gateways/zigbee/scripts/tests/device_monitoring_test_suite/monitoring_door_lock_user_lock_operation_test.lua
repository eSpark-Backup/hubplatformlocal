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
            name = "OperatingEventNotification"
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

local user_lock_operation_item_id = storage_helper:call(
    "find_item_id",
    {
        item_name = "user_lock_operation",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "door_lock",
        attribute_name = ""
    })

tests.assert_ne(user_lock_operation_item_id, nil, "Try to find item id")

-- test lock no pin codes added
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "door_lock_operating_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "keypad",
    event_name = "lock",
    user_id = 1,
    pin_code = ""
} )

tests.assert_called(
    core.update_item_value,
    "check if no update initiated",
    {}
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if door lock state requested",
    { { 42, 1, "door_lock", "LockState" } }
)

-- test unlock no pin codes added
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "door_lock_operating_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "keypad",
    event_name = "unlock",
    user_id = 1,
    pin_code = ""
} )

tests.assert_called(
    core.update_item_value,
    "check if no update initiated",
    {}
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if door lock state requested",
    { { 42, 1, "door_lock", "LockState" } }
)

-- add new pin code
events_handler( {
    event = "door_lock_programming_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "rf",
    event_name = "pin_code_added",
    user_id = 1,
    pin_code = "1111"
} )

-- test lock with correct pin
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "door_lock_operating_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "keypad",
    event_name = "lock",
    user_id = 1,
    pin_code = ""
} )

tests.assert_called(
    core.update_item_value,
    "check if user lock operation update initiated",
    {
        { user_lock_operation_item_id, { userId = 1, action = "lock" }},
        { user_lock_operation_item_id, { userId = -1, action = "unknown" }}
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if door lock state requested",
    { { 42, 1, "door_lock", "LockState" } }
)

-- test unlock with correct pin
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "door_lock_operating_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "keypad",
    event_name = "unlock",
    user_id = 1,
    pin_code = ""
} )

tests.assert_called(
    core.update_item_value,
    "check if user lock operation update initiated",
    {
        { user_lock_operation_item_id, { userId = 1, action = "unlock" }},
        { user_lock_operation_item_id, { userId = -1, action = "unknown" }}
    }
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if door lock state requested",
    { { 42, 1, "door_lock", "LockState" } }
)

-- test lock with incorrect pin
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "door_lock_operating_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "keypad",
    event_name = "lock",
    user_id = 2,
    pin_code = ""
} )

tests.assert_called(
    core.update_item_value,
    "check if no update initiated",
    {}
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if door lock state requested",
    { { 42, 1, "door_lock", "LockState" } }
)

-- test unlock with incorrect pin
core.update_item_value = tests.create_mock()
zigbee.request_attribute_value = tests.create_mock()

events_handler( {
    event = "door_lock_operating_event_notification_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "door_lock",
    event_source = "keypad",
    event_name = "unlock",
    user_id = 2,
    pin_code = ""
} )

tests.assert_called(
    core.update_item_value,
    "check if no update initiated",
    {}
)

tests.assert_called(
    zigbee.request_attribute_value,
    "check if door lock state requested",
    { { 42, 1, "door_lock", "LockState" } }
)
