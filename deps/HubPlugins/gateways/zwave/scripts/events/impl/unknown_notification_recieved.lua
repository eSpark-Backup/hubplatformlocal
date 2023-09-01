local params = ...

print(("Unknown notification received, node id: %g, channel id: %g, class id: 0x%02X, v1_alarm_type: 0x%02X, v1_alarm_level: 0x%02X, notification_type: %s, event_type: 0x%02X"):format(
        params.node_id, params.channel_id, params.class_id, params.v1_alarm_type, params.v1_alarm_level, params.notification_type, params.event_type))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
local raw_value = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/decode_base64")(params.raw_base64_value)

dev_handler:call("handle_unknown_notification",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.v1_alarm_type,
    params.v1_alarm_level,
    params.notification_type,
    params.event_type,
    raw_value
)
