local params = ...

print(string.format("Configuration value updated, node id: %g, channel id: %g, class id: %g, param num: %g, param size: %g, value: %g", params.node_id, params.channel_id, params.class_id, params.parameter_num, params.parameter_size, params.value))


local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_configuration_value", params.node_id, params.channel_id, params.class_id, params.parameter_num, params.parameter_size, params.value)
