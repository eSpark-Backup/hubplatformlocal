local self, node_id, channel_id, colors, target_colors, target_durations = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_ids.switch_color)
request_value_timer:call("stop")

return self:_super_call("default", "update_rgb_value", node_id, channel_id, colors, target_colors, target_durations)
