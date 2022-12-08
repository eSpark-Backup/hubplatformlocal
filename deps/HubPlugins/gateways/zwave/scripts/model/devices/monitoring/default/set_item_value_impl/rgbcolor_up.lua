local self, item_descriptor, value = ...

local zwave = require("zwave")

zwave.color_switch_level_change(item_descriptor.node_id, item_descriptor.channel_id, value, "up")

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start", 1200)
