local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, raw_value = ...

print("Handle notification from 12_513_10")

if notification_type == "home_security" and event_type == 0x09 then
    self:call("handle_notification_impl", node_id, channel_id, class_id, notification_type, "product_moved", nil, nil)
else
    error("Unsupported notification received")
end
