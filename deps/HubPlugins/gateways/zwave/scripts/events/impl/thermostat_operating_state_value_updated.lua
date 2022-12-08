local params = ...

print(string.format( "Thermostat operating state updated, node id: %g, channel id: %g, class id: %g", params.node_id, params.channel_id, params.class_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_thermostat_operating_state_value", params.node_id, params.channel_id, params.class_id, params.value)
