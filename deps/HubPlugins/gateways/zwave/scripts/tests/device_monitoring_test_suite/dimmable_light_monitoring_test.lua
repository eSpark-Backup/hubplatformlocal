local tests = require( "tests" )
local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require( "HUB:zwave/scripts/resources/class_ids" )
local events_handler = hubplugins.weak_loadfile( "HUB:zwave/scripts/events/events_handling" )
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils")
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")

zwave.request_node_values = tests.create_mock()
zwave.set_value = tests.create_mock()

core.update_item_value = tests.create_mock()

timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/dimmable_light_fixture")()

local function send_switch_multilevel_value(value, target_value)
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.dimmer,
        channel_id = 0,
        value = value,
        target_value = target_value
    } )
end

local function test_impl(product_info)
    -- prepare zwave
    zwave.add_node(fixture:create_node("9000", product_info))

    -- add node
    events_handler( {
        event = "node_added",
        node_id = 9000
    } )

    local dimmer_id = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 0)
    local switch_item_id = storage:call( "find_item_id", item_names.switch, 9000, class_ids.dimmer, 0)

    local timer_params = { 
        tests.skipped_param, 
        hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0, class_ids.dimmer)._timer_id, 
        "HUB:zwave/scripts/timer_handlers/common/request_value", 
        { node_id = 9000, channel_id = 0, class_id = class_ids.dimmer } 
    }

    do -- send switch multilevel update 0
        send_switch_multilevel_value( 0 )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer 0",
            { { switch_item_id, false } }
        )
    end

    do -- send switch multilevel update 5
        send_switch_multilevel_value( 5 )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer 5",
            { { switch_item_id, true }, { dimmer_id, 5 } }
        )
        tests.assert_called(
            timer.set_timeout_with_id,
            "check timer set, dimmer 5",
            { timer_params }
        )
    end

    do -- send switch multilevel update 99
        send_switch_multilevel_value( 99 )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer 99",
            { { switch_item_id, true }, { dimmer_id, 100 } }
        )
        tests.assert_called(
            timer.set_timeout_with_id,
            "check timer set, dimmer 99",
            {}
        )
    end

    do -- send switch multilevel update from 0 to 99
        send_switch_multilevel_value( 0, 99 )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer from 0 to 99",
            { { switch_item_id, false }, { dimmer_id, 0 } }
        )
        tests.assert_called(
            timer.set_timeout_with_id,
            "check timer set, dimmer from 0 to 99",
            { timer_params }
        )
    end

    do -- send switch multilevel update from 50 to 99
        send_switch_multilevel_value( 50, 99 )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer from 50 to 99",
            { { switch_item_id, true }, { dimmer_id, 50 } }
        )
        tests.assert_called(
            timer.set_timeout_with_id,
            "check timer set, dimmer from 50 to 99",
            { timer_params }
        )
    end

    do -- send switch multilevel update from 99 to 99
        send_switch_multilevel_value( 99, 99 )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer from 99 to 99",
            { { switch_item_id, true }, { dimmer_id, 100 } }
        )
        tests.assert_called(
            timer.set_timeout_with_id,
            "check timer set, dimmer from 99 to 99",
            {}
        )
    end

    do -- send switch multilevel update 0xFF
        send_switch_multilevel_value( 0xFF )

        tests.assert_called(
            core.update_item_value,
            "check updated items, dimmer 0xFF",
            { { switch_item_id, true }, { dimmer_id, 100 } }
        )
        tests.assert_called(
            timer.set_timeout_with_id,
            "check timer set, dimmer 0xFF",
            {}
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
