local self, node_id, channel_id, class_id, v1_alarm_level = ...

print("doorlook alarm 0x11: RF not fully locked operation")

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "rf_not_fully_locked_operation")
