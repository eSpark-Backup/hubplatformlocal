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
    if params.item_initial_value then
        original_update_item_value(zone_status_item_id, params.item_initial_value)
    end

    core.update_item_value = tests.create_mock()
    zigbee.send_command = tests.create_mock()

    local house_modes_events_handler = assert( loadfile( "HUB:zigbee/scripts/events/house_modes/events_handling" ) )

    house_modes_events_handler( {
        event = "house_mode_switched",
        from = params.from,
        to = params.to,
        status = params.status,
        switch_delay = params.switch_delay
    } )

    tests.assert_called(
        core.update_item_value,
        "check if item value updated",
        params.items_updates
    )

    tests.assert_called(
        zigbee.send_command,
        "check if command sent",
        params.commands_sent
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                item_initial_value = "disarmed",
                to = "1",
                status = "begin",
                switch_delay = 0,
                items_updates = {},
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "panel_disarmed", seconds_remaining = 0} }
                }
            },
            description = "test with initial disarmed and home begin"
        },
        {
            value = {
                item_initial_value = "stay",
                to = "1",
                status = "begin",
                switch_delay = 0,
                items_updates = {},
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "armed_stay", seconds_remaining = 0} }
                }
            },
            description = "test with initial stay and home begin"
        },
        {
            value = {
                item_initial_value = "disarmed",
                to = "1",
                status = "done",
                switch_delay = 0,
                items_updates = {},
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "panel_disarmed", seconds_remaining = 0} }
                }
            },
            description = "test with initial disarmed and home done"
        },
        {
            value = {
                item_initial_value = "stay",
                to = "1",
                status = "done",
                switch_delay = 0,
                items_updates = {},
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "armed_stay", seconds_remaining = 0} }
                }
            },
            description = "test with initial stay and home done"
        },
        {
            value = {
                item_initial_value = "stay",
                to = "2",
                status = "begin",
                switch_delay = 30,
                items_updates = { { zone_status_item_id, "away" } },
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "arming_away", seconds_remaining = 30} }
                }
            },
            description = "test with away begin"
        },
        {
            value = {
                item_initial_value = "stay",
                to = "2",
                status = "done",
                switch_delay = 0,
                items_updates = { { zone_status_item_id, "away" } },
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "armed_away", seconds_remaining = 0} }
                }
            },
            description = "test with away done"
        },
        {
            value = {
                item_initial_value = "stay",
                to = "3",
                status = "begin",
                switch_delay = 30,
                items_updates = { { zone_status_item_id, "night" } },
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "arming_night", seconds_remaining = 30} }
                }
            },
            description = "test with night begin"
        },
        {
            value = {
                item_initial_value = "stay",
                to = "3",
                status = "done",
                switch_delay = 0,
                items_updates = { { zone_status_item_id, "night" } },
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "armed_night", seconds_remaining = 0} }
                }
            },
            description = "test with night done"
        },
        {
            value = {
                item_initial_value = "away",
                from = "1",
                to = "2",
                status = "cancel",
                switch_delay = 0,
                items_updates = { { zone_status_item_id, "disarmed" } },
                commands_sent = {
                    { 42, 1, "ias_ace", "PanelStatusChanged", {panel_status = "panel_disarmed", seconds_remaining = 0} }
                }
            },
            description = "test with away cancel"
        }
    }
)
