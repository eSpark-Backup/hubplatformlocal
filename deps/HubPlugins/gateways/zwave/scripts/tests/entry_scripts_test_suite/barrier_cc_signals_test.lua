local zwave = require("zwave")
local events_handler = assert( loadfile( "HUB:zwave/scripts/events/events_handling" ) )
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local tests = require("tests")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local set_item_value = loadfile( "HUB:zwave/scripts/set_item_value" )
local item_names = require("HUB:zwave/scripts/resources/item_names")

zwave.request_node_values = tests.create_mock()

-- add node
local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/barrier_with_signals_fixture")()


local function test_impl(params)
    local switch_item_id = storage:call( 
        "find_item_id", 
        item_names.switch, 
        9000, 
        class_ids.barrier_operator, 
        0, 
        params.signal_name
    )
    assert(switch_item_id, "Unable to find switch item")

    do -- set false
        zwave.set_barrier_signal_value = tests.create_mock()
        zwave.request_barrier_signal_value = tests.create_mock()
        set_item_value( { item_id = switch_item_id, value = false } )

        tests.assert_called(
            zwave.set_barrier_signal_value,
            "check barrier signal seted",
            { { "9000", 0, params.signal_name, 0 } }
        )
        
        tests.assert_called(
            zwave.request_barrier_signal_value,
            "check barrier signal requested",
            { { "9000", 0, params.signal_name } }
        )
    end

    do -- set true
        zwave.set_barrier_signal_value = tests.create_mock()
        zwave.request_barrier_signal_value = tests.create_mock()
        set_item_value( { item_id = switch_item_id, value = true } )

        tests.assert_called(
            zwave.set_barrier_signal_value,
            "check barrier signal seted",
            { { "9000", 0, params.signal_name, 0xFF } }
        )
        
        tests.assert_called(
            zwave.request_barrier_signal_value,
            "check barrier signal requested",
            { { "9000", 0, params.signal_name } }
        )
    end
end

tests.run_with_params( 
    test_impl, 
    {
        {
            value = {
                signal_name = "audible_notification",
            },
            description = "test with audible notification"
        },
        {
            value = {
                signal_name = "visual_notification",
            },
            description = "test with visual notification"
        }
    }
)
