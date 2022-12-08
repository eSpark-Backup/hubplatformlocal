local self, node_id, channel_id, class_id, user_id = ...

local are_all_user_codes_deleted = user_id == 0xFF

if are_all_user_codes_deleted then
    print("doorlock alarm 0x21: all user code deleted")
    self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "all_user_codes_deleted")
else
    print("doorlook alarm 0x21: single user code deleted " .. user_id)
    self:call("forward_access_control_notification_event", node_id, channel_id, class_id, "single_user_code_deleted", user_id)
end
