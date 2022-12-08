local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )
local user_code_actions = require("HUB:zigbee/scripts/resources/enums/user_code_actions")

house_modes.get_current = function() return "1" end

local cluster_ias_ace = {
    name = "ias_ace",
    data_holder = "client",
    attributes = {},
    commands = {
        {
            name = "Arm"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_ias_ace }
}

local node = {
    id = 42,
    manufacturer_id = "CentraLite",
    model_id = "3400",
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
local user_codes_item_id = storage:call(
    "find_item_id",
    {
        item_name = "user_codes",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(user_codes_item_id, nil, "Try to find item id")

local item_id = storage:call(
    "find_item_id",
    {
        item_name = "user_code_action",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")


-- test no user codes specified
core.update_item_value = tests.create_mock()

events_handler( {
    event = "arm_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_ace",
    pin_code = "1111"
} )

tests.assert_called(
    core.update_item_value,
    "check if item value updated",
    {
        { item_id, { userId = -1, action = user_code_actions.invalid_user_code_entered } },
        { item_id, { userId = -1, action = user_code_actions.unknown } }
    }
)

-- add user codes
core.update_item_dictionary_value(user_codes_item_id, "1", { code = "1111", name = "test1" })
core.update_item_dictionary_value(user_codes_item_id, "2", { code = "2222", name = "test2" })

-- test wrong user code
core.update_item_value = tests.create_mock()

events_handler( {
    event = "arm_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_ace",
    pin_code = "3333"
} )

tests.assert_called(
    core.update_item_value,
    "check if item value updated",
    {
        { item_id, { userId = -1, action = user_code_actions.invalid_user_code_entered } },
        { item_id, { userId = -1, action = user_code_actions.unknown } }
    }
)

-- test correct user code
core.update_item_value = tests.create_mock()

events_handler( {
    event = "arm_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_ace",
    pin_code = "2222"
} )

tests.assert_called(
    core.update_item_value,
    "check if item value updated",
    {
        { item_id, { userId = 2, action = user_code_actions.user_code_entered } },
        { item_id, { userId = -1, action = user_code_actions.unknown } }
    }
)
