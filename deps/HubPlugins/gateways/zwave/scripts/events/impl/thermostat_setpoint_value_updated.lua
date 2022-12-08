local params = ...

print(string.format( "Thermostat setpoint value updated, node id: %g, channel id: %g, class id: %g", params.node_id, params.channel_id, params.class_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call(
    "update_thermostat_setpoint_value", 
    params.node_id, 
    params.channel_id, 
    params.class_id, 
    params.type, 
    params.value, 
    params.scale,
    params.is_requested
)
