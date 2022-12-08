local self, node_id, channel_id, class_id, sensor_type, value, delay = ...

if sensor_type == "general" then
    -- do nothing
else
    self:_super_call("default", "update_alarm_sensor_value", node_id, channel_id, class_id, sensor_type, value, delay)
end
