local tests = require( "tests" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils")
local zwave = require("zwave")
local core = require("core")

zwave.request_node_values = tests.create_mock()

local function send_barrier_signal_value(signal_name, value)
    events_handler( {
        event = "barrier_signal_value_updated",
        node_id = 9000,
        class_id = class_ids.barrier_operator,
        channel_id = 0,
        signal_name = signal_name,
        value = value
    } )
end

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

    do -- send signal value update 0
        core.update_item_value = tests.create_mock()
        send_barrier_signal_value( params.signal_name, 0 )

        tests.assert_called(
            core.update_item_value,
            "check updated items",
            { { switch_item_id, false } }
        )
    end

    do -- send signal value update 5
        core.update_item_value = tests.create_mock()
        send_barrier_signal_value( params.signal_name, 5 )

        tests.assert_called(
            core.update_item_value,
            "check updated items",
            { { switch_item_id, false } }
        )
    end

    do -- send signal update 0xFF
        core.update_item_value = tests.create_mock()
        send_barrier_signal_value( params.signal_name, 0xFF )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer 99",
            { { switch_item_id, true } }
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
