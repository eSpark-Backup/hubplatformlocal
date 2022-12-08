local params = ...

print(string.format("Node firmware update status, node id: %g, target: %s, status: %s, progress: %s, error: %s", params.node_id, tostring(params.target), params.status, tostring(params.progress), params.error))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("handle_node_firmware_update_status", params.node_id, params.image, params.target, params.status, params.progress, params.error)
