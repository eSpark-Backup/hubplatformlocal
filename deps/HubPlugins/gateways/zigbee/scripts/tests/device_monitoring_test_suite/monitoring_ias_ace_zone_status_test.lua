local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )
local user_code_actions = require("HUB:zigbee/scripts/resources/enums/user_code_actions")
local original_update_item_value = core.update_item_value

house_modes.switch = tests.create_mock()
house_modes.get_current = tests.create_mock()
house_modes.cancel_switch = tests.create_mock()

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

local action_item_id = storage:call(
    "find_item_id",
    {
        item_name = "user_code_action",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(action_item_id, nil, "Try to find item id")

local zone_status_item_id = storage:call(
    "find_item_id",
    {
        item_name = "zones_status",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(zone_status_item_id, nil, "Try to find item id")


-- test no user codes specified
core.update_item_value = tests.create_mock()
zigbee.send_command = tests.create_mock()

events_handler( {
    event = "arm_command_received",
    node_id = 42,
    endpoint_id = 1,
    cluster_name = "ias_ace",
    arm_mode = "arm_all_zones",
    pin_code = "1111"
} )

tests.assert_called(
    core.update_item_value,
    "check if item value updated",
    {
        { action_item_id, { userId = -1, action = user_code_actions.invalid_user_code_entered } },
        { action_item_id, { userId = -1, action = user_code_actions.unknown } }
    }
)

tests.assert_called(
    zigbee.send_command,
    "check if response sent",
    {
        { 42, 1, "ias_ace", "ArmResponse", "invalid_arm_disarm_code" }
    }
)

-- add user codes
core.update_item_dictionary_value(user_codes_item_id, "1", { code = "1111", name = "test1" })
core.update_item_dictionary_value(user_codes_item_id, "2", { code = "2222", name = "test2" })


local function test_impl(params)
    if params.item_initial_value then
        original_update_item_value(zone_status_item_id, params.item_initial_value)
    end

    core.update_item_value = tests.create_mock()
    zigbee.send_command = tests.create_mock()

    events_handler( {
        event = "arm_command_received",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        arm_mode = params.arm_mode,
        pin_code = params.pin_code
    } )

    tests.assert_called(
        core.update_item_value,
        "check if item value updated",
        params.items_updates
    )

    tests.assert_called(
        zigbee.send_command,
        "check if response sent",
        params.commands_sent
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                arm_mode = "arm_all_zones",
                pin_code = "3333",
                items_updates = {
                    { action_item_id, { userId = -1, action = user_code_actions.invalid_user_code_entered } },
                    { action_item_id, { userId = -1, action = user_code_actions.unknown } }
                },
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "invalid_arm_disarm_code" }
                }
            },
            description = "test with invalid code"
        },
        {
            value = {
                arm_mode = "disarm",
                pin_code = "1111",
                items_updates = {
                    { action_item_id, { userId = 1, action = user_code_actions.user_code_entered } },
                    { action_item_id, { userId = -1, action = user_code_actions.unknown } },
                    { zone_status_item_id, "disarmed" }
                },
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "already_disarmed" }
                }
            },
            description = "try to disarm failed"
        },
        {
            value = {
                arm_mode = "arm_all_zones",
                pin_code = "1111",
                items_updates = {
                    { action_item_id, { userId = 1, action = user_code_actions.user_code_entered } },
                    { action_item_id, { userId = -1, action = user_code_actions.unknown } },
                    { zone_status_item_id, "away" }
                },
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "all_zones_armed" },
                }
            },
            description = "test with arm all zones"
        },
        {
            value = {
                arm_mode = "arm_day_home_zones_only",
                pin_code = "1111",
                items_updates = {
                    { action_item_id, { userId = 1, action = user_code_actions.user_code_entered } },
                    { action_item_id, { userId = -1, action = user_code_actions.unknown } },
                    { zone_status_item_id, "stay" }
                },
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "only_day_home_zones_armed" },
                }
            },
            description = "test with arm day/home zones"
        },
        {
            value = {
                arm_mode = "arm_night_home_zones_only",
                pin_code = "1111",
                items_updates = {
                    { action_item_id, { userId = 1, action = user_code_actions.user_code_entered } },
                    { action_item_id, { userId = -1, action = user_code_actions.unknown } },
                    { zone_status_item_id, "night" }
                },
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "only_night_sleep_zones_armed" },
                }
            },
            description = "test with arm night/home zones"
        },
        {
            value = {
                arm_mode = "disarm",
                pin_code = "1111",
                item_initial_value = "armed_away",
                items_updates = {
                    { action_item_id, { userId = 1, action = user_code_actions.user_code_entered } },
                    { action_item_id, { userId = -1, action = user_code_actions.unknown } },
                    { zone_status_item_id, "disarmed" }
                },
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "all_zones_disarmed" },
                }
            },
            description = "test with disarm"
        }
    }
)
