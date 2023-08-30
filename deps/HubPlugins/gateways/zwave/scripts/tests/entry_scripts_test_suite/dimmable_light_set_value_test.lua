local tests = require( "tests" )

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils")
local zwave = require( "zwave" )
local timer = require( "timer" )

zwave.request_node_values = tests.create_mock()
zwave.set_value = tests.create_mock()

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()

local fixture = hubplugins.weak_loadfile( "HUB:zwave/scripts/tests/fixtures/dimmable_light_fixture" )()

local function test_impl( product_info )
    -- prepare zwave
    zwave.add_node( fixture:create_node( "9000", product_info ) )

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    local dimmer_id = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 0 )
    local switch_item_id = storage:call( "find_item_id", item_names.switch, 9000, class_ids.dimmer, 0 )

    local expected_timer_call_params = { tests.skipped_param, tests.skipped_param, "HUB:zwave/scripts/timer_handlers/common/request_value", { node_id = "9000", channel_id = 0, class_id = class_ids.dimmer } }

    do -- set dimmer 0
        hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = dimmer_id, value = 0 })

        tests.assert_called(zwave.set_value, "set dimmer 0, check dimmer set", { { "9000", class_ids.dimmer, 0, 0 } } )
        tests.assert_called(
            timer.set_timeout_with_id,
            "set dimmer 0, check timer set",
            { expected_timer_call_params }
        )
    end

    do -- set dimmer 50
        hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = dimmer_id, value = 50 })

        tests.assert_called(zwave.set_value, "set dimmer 50, check dimmer set", { { "9000", class_ids.dimmer, 0, 50 } } )
        tests.assert_called(
            timer.set_timeout_with_id,
            "set dimmer , check timer set",
            { expected_timer_call_params }
        )
    end

    do -- set dimmer 100
        hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = dimmer_id, value = 100 })

        tests.assert_called(zwave.set_value, "set dimmer 100, check dimmer set", { { "9000", class_ids.dimmer, 0, 99 } } )
        tests.assert_called(
            timer.set_timeout_with_id,
            "set dimmer 100, check timer set",
            { expected_timer_call_params }
        )
    end

    do --set switch on
        hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_item_id, value = true })

        tests.assert_called(zwave.set_value, "set switch on, check dimmer set", { { "9000", class_ids.dimmer, 0, 0xFF } } )
        tests.assert_called(
            timer.set_timeout_with_id,
            "set switch on, check timer set",
            { expected_timer_call_params }
        )
    end

    do --set switch off
        hubplugins.weak_loadfile("HUB:zwave/scripts/set_item_value")({ item_id = switch_item_id, value = false })

        tests.assert_called(zwave.set_value, "set switch off, check dimmer set", { { "9000", class_ids.dimmer, 0, 0x00 } } )
        tests.assert_called(
            timer.set_timeout_with_id,
            "set switch off, check timer set",
            { expected_timer_call_params }
        )
    end

    -- remove node
    events_handler( {
        event = "node_removed",
        node_id = "9000",
        manufacturer_id = product_info.manufacturer_id,
        product_type = product_info.product_type,
        product_id = product_info.product_id
    } )
end

tests.run_with_params( test_impl, fixture:get_manufacturer_specifics() )
tests.run_with_params( test_impl, fixture:get_autodetection_specifics() )