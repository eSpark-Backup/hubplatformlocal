local params = ...

print(string.format("Thermostat setback value updated, node id: %g, channel id: %g, override type: %s, mode: %s, value: %s", params.node_id, params.channel_id, params.override_type, params.mode, tostring(params.value)))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_thermostat_setback_value", params.node_id, params.channel_id, params.override_type, params.mode, params.value)
