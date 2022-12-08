local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

if channel_id == 0 then
    self:_super_call("default", "update_multilevel_switch", node_id, channel_id, class_id, value, target_value, target_duration)
end
