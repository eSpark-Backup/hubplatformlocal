local self, node_id, channel_id, class_id, user_id = ...

print("doorlook alarm 0x12: keypad lock by user " .. user_id)

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "keypad_lock_operation", user_id)
