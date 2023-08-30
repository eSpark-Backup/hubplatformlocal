local self, node_id, channel_id, class_id, user_id = ...

print("doorlook alarm 0x84: Valid user code usage but outside of schedule, user " .. user_id)

self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "valid_user_but_outside_of_schedule_operation", user_id)
