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


local function test_impl(params)
    zigbee.send_command = tests.create_mock()

    local house_modes_events_handler = assert( loadfile( "HUB:zigbee/scripts/events/house_modes/events_handling" ) )

    house_modes_events_handler( {
        event = "house_mode_alarmed",
        house_mode_id = params.house_mode_id
    } )

    if params.send_entry_delay then
        tests.assert_called(
            zigbee.send_command,
            "check if command sent",
            { { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "entry_delay", seconds_remaining = 30} } }
        )
    else
        tests.assert_called(
            zigbee.send_command,
            "check if command not sent",
            {}
        )
    end
end

tests.run_with_params(
    test_impl,
    {
        {
            value = { house_mode_id = "1" },
            description = "test with house mode 1"
        },
        {
            value = { house_mode_id = "2", send_entry_delay = true },
            description = "test with house mode 1"
        },
        {
            value = { house_mode_id = "3" },
            description = "test with house mode 1"
        },
        {
            value = { house_mode_id = "4" },
            description = "test with house mode 1"
        }
    }
)
