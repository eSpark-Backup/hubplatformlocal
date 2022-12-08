local params = ...

print(string.format( "Thermostat mode updated, node id: %g, channel id: %g, class id: %g, mode: %g", params.node_id, params.channel_id, params.class_id, params.mode))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_thermostat_mode", params.node_id, params.channel_id, params.class_id, params.mode, params.manufacturer_specific_data)
