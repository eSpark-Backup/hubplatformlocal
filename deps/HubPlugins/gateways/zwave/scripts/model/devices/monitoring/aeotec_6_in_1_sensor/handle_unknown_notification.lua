local self, node_id, channel_id, class_id, v1_alarm_type, v1_alarm_level, notification_type, event_type, raw_value = ...

if event_type == 0xFE then
    return
end

local events = {
    [0x00] = "state_idle",
    [0x03] = "product_cover_removed",
    [0x08] = "motion_detected"
}

self:call("handle_notification_impl", node_id, channel_id, class_id, notification_type, events[event_type], nil, nil)
