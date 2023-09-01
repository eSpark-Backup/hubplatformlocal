local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, notification_value, raw_value = ...

if v1_alarm_type ~= 0x00 and self:call("try_handle_v1_alarm", node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level) then
    print("[handler.handle_notification]: handled v1 alarm")
    return
end

self:call("handle_notification_impl", node_id, channel_id, class_id, notification_type, event_type, notification_value, raw_value)
