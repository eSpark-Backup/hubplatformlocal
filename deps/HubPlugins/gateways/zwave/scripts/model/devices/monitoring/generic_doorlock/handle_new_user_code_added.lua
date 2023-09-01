local self, node_id, channel_id, class_id, user_id = ...

print("doorlook alarm 0x70: New User code added, user " .. user_id)

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "new_user_code_added", user_id)
