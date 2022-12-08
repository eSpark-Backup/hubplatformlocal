local self, node_id, channel_id, class_id, value, target_value, duration = ...

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_id)
request_value_timer:call("stop")

self:_super_call("default", "update_value", node_id, channel_id, class_id, value, target_value, duration)
