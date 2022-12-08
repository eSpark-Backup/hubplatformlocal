local self, node_id, channel_id, class_id, notification_type, event_type, notification_value, raw_value = ...

if notification_type == "home_security" and event_type == "intrusion" then
    event_type = "state_idle"
    notification_value = nil
    raw_value = nil
end

self:_super_call("default", "handle_notification_impl", node_id, channel_id, class_id, notification_type, event_type, notification_value, raw_value)
