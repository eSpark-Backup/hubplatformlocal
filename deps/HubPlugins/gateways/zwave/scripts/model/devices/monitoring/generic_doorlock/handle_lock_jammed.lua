local self, node_id, channel_id, class_id, v1_alarm_level = ...

print("doorlook alarm 0x09: lock jammed")

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "lock_jammed")
