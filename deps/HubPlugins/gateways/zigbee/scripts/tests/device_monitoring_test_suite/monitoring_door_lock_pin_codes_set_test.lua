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

-- test add value
zigbee.send_command = tests.create_mock()
storage.set_table = tests.create_mock()
core.generate_item_dictionary_number_id = function(...)
    return 1
end

loadfile( "HUB:zigbee/scripts/add_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        value = {
            code = "1111",
            name = "test1"
        }
    }
)

tests.assert_called(
    zigbee.send_command,
    "check if command sent",
    { { 42, 1, "door_lock", "SetPinCode", { user_id = 1, pin_code = "1111", user_status = "occupied_enabled" } } }
)

tests.assert_called(
    storage.set_table,
    "check if user name saved",
    { { "usernames." .. item_id, {["1"] = "test1"} } }
)

-- test set value
zigbee.send_command = tests.create_mock()
storage.set_table = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        key = "2",
        value = {
            code = "2222",
            name = "test2"
        }
    }
)

tests.assert_called(
    zigbee.send_command,
    "check if command sent",
    { { 42, 1, "door_lock", "SetPinCode", { user_id = 2, pin_code = "2222", user_status = "occupied_enabled" } } }
)

tests.assert_called(
    storage.set_table,
    "check if user name saved",
    { { "usernames." .. item_id, {["2"] = "test2"} } }
)

-- test set value enabled
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        key = "2",
        value = {
            code = "2222",
            name = "test2",
            mode = "enabled"
        }
    }
)

tests.assert_called(
    zigbee.send_command,
    "check if command sent",
    { { 42, 1, "door_lock", "SetPinCode", { user_id = 2, pin_code = "2222", user_status = "occupied_enabled" } } }
)

-- test set value disabled
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/set_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        key = "2",
        value = {
            code = "2222",
            name = "test2",
            mode = "disabled"
        }
    }
)

tests.assert_called(
    zigbee.send_command,
    "check if command sent",
    { { 42, 1, "door_lock", "SetPinCode", { user_id = 2, pin_code = "2222", user_status = "occupied_disabled" } } }
)

-- test remove value
zigbee.send_command = tests.create_mock()

loadfile( "HUB:zigbee/scripts/remove_item_dictionary_value" )(
    {
        item_id = item_id,
        value_type = "userCode",
        key = "1"
    }
)

tests.assert_called(
    zigbee.send_command,
    "check if command sent",
    { { 42, 1, "door_lock", "ClearPinCode", 1 } }
)
