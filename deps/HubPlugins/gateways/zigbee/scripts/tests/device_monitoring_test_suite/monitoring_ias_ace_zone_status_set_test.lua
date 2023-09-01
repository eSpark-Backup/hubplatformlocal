local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )

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
    core.update_item_value = tests.create_mock()
    zigbee.send_command = tests.create_mock()

    loadfile( "HUB:zigbee/scripts/set_item_value" )(
        {
            item_id = zone_status_item_id,
            value = params.item_value
        }
    )

    tests.assert_called(
        core.update_item_value,
        "check if item value updated",
        {
            { zone_status_item_id, params.item_value }
        }
    )

    tests.assert_called(
        zigbee.send_command,
        "check if panel status changed sent",
        {
            { 42, 1, "ias_ace", "PanelStatusChanged", params.panel_status }
        }
    )
end

tests.run_with_params(
    test_impl,
    {
        {
            value = {
                item_value = "disarmed",
                panel_status = "panel_disarmed"
            },
            description = "test with disarmed"
        },
        {
            value = {
                item_value = "stay",
                panel_status = "armed_stay"
            },
            description = "test with armed stay"
        },
        {
            value = {
                item_value = "night",
                panel_status = "armed_night"
            },
            description = "test with armed night"
        },
        {
            value = {
                item_value = "away",
                panel_status = "armed_away"
            },
            description = "test with armed away"
        }
    }
)
