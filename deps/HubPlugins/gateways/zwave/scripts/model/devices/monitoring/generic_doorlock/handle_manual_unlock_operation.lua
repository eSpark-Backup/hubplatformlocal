local self, node_id, channel_id, class_id, v1_alarm_level = ...

print("doorlook alarm 0x16: manual unlock (by cylinder or inside thumb turn)")

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "manual_unlock_operation")
