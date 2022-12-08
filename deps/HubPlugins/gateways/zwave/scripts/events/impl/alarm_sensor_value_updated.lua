local params = ...

print(string.format("Alarm sensor value updated, node id: %g, channel id: %g, class id: %g", params.node_id, params.channel_id, params.class_id))
local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)

dev_handler:call("update_alarm_sensor_value",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.sensor_type,
    params.value,
    params.delay
)
