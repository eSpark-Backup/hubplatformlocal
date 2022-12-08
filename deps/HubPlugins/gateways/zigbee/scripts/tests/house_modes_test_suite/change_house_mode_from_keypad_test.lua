local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )
local user_code_actions = require("HUB:zigbee/scripts/resources/enums/user_code_actions")
local original_update_item_value = core.update_item_value

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

-- add user codes
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

core.update_item_dictionary_value(user_codes_item_id, "1", { code = "1111", name = "test1" })
core.update_item_dictionary_value(user_codes_item_id, "2", { code = "2222", name = "test2" })


local function test_impl(params)
    core.update_item_value = tests.create_mock()
    zigbee.send_command = tests.create_mock()
    house_modes.switch = tests.create_mock()
    house_modes.cancel_switch = tests.create_mock()

    house_modes.get_current = function()
        return params.initial_house_mode
    end

    events_handler( {
        event = "arm_command_received",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        arm_mode = params.arm_mode,
        pin_code = params.pin_code
    } )

    tests.assert_called(
        house_modes.cancel_switch,
        "check if house mode canceled",
        1
    )

    tests.assert_called(
        house_modes.switch,
        "check if house mode updated",
        params.house_modes
    )

    if params.commands_sent then
        tests.assert_called(
            zigbee.send_command,
            "check if command sent",
            params.commands_sent
        )
    end
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                arm_mode = "disarm",
                pin_code = "1111",
                initial_house_mode = "1",
                house_modes = {},
                commands_sent = {
                    { 42, 1, "ias_ace", "ArmResponse", "already_disarmed" },
                    { 42, 1, "ias_ace", "PanelStatusChanged", { panel_status = "panel_disarmed" } }
                }
            },
            description = "test with no house mode change"
        },
        {
            value = {
                arm_mode = "disarm",
                pin_code = "1111",
                initial_house_mode = "2",
                house_modes = {{"1"}},
            },
            description = "test with disarm"
        },
        {
            value = {
                arm_mode = "arm_all_zones",
                pin_code = "1111",
                initial_house_mode = "1",
                house_modes = {{"2"}},
            },
            description = "test with arm all zones"
        },
        {
            value = {
                arm_mode = "arm_day_home_zones_only",
                pin_code = "1111",
                initial_house_mode = "2",
                house_modes = {{"1"}},
            },
            description = "test with arm day/home zones"
        },
        {
            value = {
                arm_mode = "arm_night_home_zones_only",
                pin_code = "1111",
                initial_house_mode = "2",
                house_modes = {{"3"}},
            },
            description = "test with arm night/home zones"
        }
    }
)
