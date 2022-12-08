local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

self:call("update_value", node_id, channel_id, class_id, value)

self:_super_call("default", "update_multilevel_switch", node_id, channel_id, class_id, value, target_value, target_duration)
