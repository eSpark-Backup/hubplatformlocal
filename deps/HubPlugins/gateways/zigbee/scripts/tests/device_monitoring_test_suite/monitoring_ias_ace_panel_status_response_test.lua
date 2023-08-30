local tests = require( "tests" )
local core = require( "core" )
local house_modes = require( "house_modes" )
local emergency_state = loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

house_modes.get_current = function() return "1" end

local cluster_ias_ace = {
    name = "ias_ace",
    data_holder = "client",
    attributes = {},
    commands = {
        {
            name = "Arm"
        },
        {
            name = "Emergency"
        },
        {
            name = "Panic"
        }
    }
}

local endpoint_1 = {
    id = 1,
    clusters = { cluster_ias_ace }
}

local node = {
    id = 42,
    manufacturer_id = "Ezlo",
    model_id = "Keypad",
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
local item_id = storage:call(
    "find_item_id",
    {
        item_name = "zones_status",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        attribute_name = ""
    })

tests.assert_ne(item_id, nil, "Try to find item id")


local function test_response(params)
    core.update_item_value(item_id, params.zone_status)

    zigbee.send_command = tests.create_mock()

    events_handler( {
        event = "simple_command_received",
        node_id = 42,
        endpoint_id = 1,
        cluster_name = "ias_ace",
        command_name = "GetPanelStatus"
    } )

    tests.assert_called(
        zigbee.send_command,
        "check if panel status response sent",
        {
            { 42, 1, "ias_ace", "GetPanelStatusResponse", params.panel_status }
        }
    )
end

tests.run_with_params(
    test_response,
    {
        {
            value = {
                zone_status = "disarmed",
                panel_status = "panel_disarmed"
            },
            description = "test with disarmed"
        },
        {
            value = {
                zone_status = "stay",
                panel_status = "armed_stay"
            },
            description = "test with armed stay"
        },
        {
            value = {
                zone_status = "night",
                panel_status = "armed_night"
            },
            description = "test with armed night"
        },
        {
            value = {
                zone_status = "away",
                panel_status = "armed_away"
            },
            description = "test with armed away"
        }
    }
)
