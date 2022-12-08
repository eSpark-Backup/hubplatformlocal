local params = ...

print(string.format("Master code updated, node id: %g, channel id: %g, class id: %g, code: %s", params.node_id, params.channel_id, params.class_id, params.value))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_master_code", params.node_id, params.channel_id, params.class_id, params.value)
