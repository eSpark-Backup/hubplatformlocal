local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, raw_value = ...

if not self:call("try_handle_v1_alarm", node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level) then
    print("TODO: implement handler.handle_unknown_notification")
end
