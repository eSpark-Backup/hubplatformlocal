local params = ...

print(string.format( "Float value updated, node id: %g, channel id: %g, class id: %g, value: %g, scale: %s ", params.node_id, params.channel_id, params.class_id, params.value, params.scale))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_float_value", params.node_id, params.channel_id, params.class_id, params.type, params.value, params.scale, params.precision)
