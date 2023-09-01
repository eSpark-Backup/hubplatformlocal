local self, item_descriptor, value, item_id = ...

local zwave = require("zwave")

local helper = hubplugins.weak_require("HUB:zwave/scripts/helpers/multilevel_switch_helper")
value = helper.value_from_ui(value)

zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start")

local estimated_value_timer = hubplugins.object("estimated_value_timer", "HUB:zwave/scripts/timer_handlers", item_id)
estimated_value_timer:call("stop")
