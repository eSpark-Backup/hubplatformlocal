local self, node_id, channel_id, class_id, user_id = ...

print("doorlook alarm 0x71: New User code not added due to duplicated code, user " .. user_id)

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "new_user_code_not_added_due_to_duplicated_code", user_id)
