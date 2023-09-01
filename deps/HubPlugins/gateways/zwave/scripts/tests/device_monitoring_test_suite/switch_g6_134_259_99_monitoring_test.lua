local tests = require( "tests" )

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local events_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/events/events_handling")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local zwave = require("zwave")
local core = require("core")
local timer = require("timer")

zwave.request_node_values = tests.create_mock()

core.update_item_value = tests.create_mock()
timer.set_timeout_with_id = tests.create_mock()
timer.cancel = tests.create_mock()

local fixture = hubplugins.weak_loadfile("HUB:zwave/scripts/tests/fixtures/switch_g6_134_259_99_fixture")()

local node = fixture:create_node("9000")

fixture:add_node(node)

local function send_switch_multilevel_value(value, channel_id)
    events_handler( {
        event = "value_updated",
        node_id = 9000,
        class_id = class_ids.dimmer,
        channel_id = channel_id,
        value = value
    } )
end

local dimmer_id = storage:call("find_item_id", item_names.dimmer, 9000, class_ids.dimmer, 0)

local timer_params = { 
    tests.skipped_param, 
    hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", 9000, 0, class_ids.dimmer)._timer_id, 
    "HUB:zwave/scripts/timer_handlers/common/request_value", 
    { node_id = 9000, channel_id = 0, class_id = class_ids.dimmer } 
}

do -- send switch multilevel update by zero channel
    send_switch_multilevel_value(50, 0)

    tests.assert_called(
        timer.set_timeout_with_id,
        "check timer set. channel: 0, value: 50",
        { timer_params }
    )

    tests.assert_called(core.update_item_value, "check updated items. channel: 0, value: 50", { { dimmer_id, 50 } })
end

do -- send switch multilevel update by 1st channel
    send_switch_multilevel_value(50, 1)

    tests.assert_called(timer.set_timeout_with_id, "check timer set. channel: 1, value: 50", nil, 0)
    tests.assert_called(core.update_item_value, "check updated items. channel: 1, value: 50", nil, 0)
end

do -- send switch multilevel update by 2nd channel
    send_switch_multilevel_value(50, 2)

    tests.assert_called(timer.set_timeout_with_id, "check timer set. channel: 2, value: 50", nil, 0)
    tests.assert_called(core.update_item_value, "check updated items. channel: 2, value: 50", nil, 0)
end


fixture:remove_node(node)
