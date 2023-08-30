local params = ...

print(("Notification received, node id: %g, channel id: %g, class id: 0x%02X "):format(
        params.node_id, params.channel_id, params.class_id))

if params.v1_alarm_type and params.v1_alarm_level then
    print(("      received v1_alarm_type: 0x%02X, v1_alarm_level: 0x%02X"):format(
        params.v1_alarm_type, params.v1_alarm_level))
end

if params.notification_type and params.event_type then
    print(("      received notification_type: %s, event_type: %s"):format(
        params.notification_type, params.event_type, tostring(params.value)))
end

if params.value then
    print(("      received value: %s"):format( tostring(params.value)))
end

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
local raw_value = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/decode_base64")(params.raw_base64_value)

dev_handler:call("handle_notification",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.v1_alarm_type,
    params.v1_alarm_level,
    params.notification_type,
    params.event_type,
    params.value,
    raw_value
)
