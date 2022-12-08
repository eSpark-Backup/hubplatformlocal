local params = ...

print(string.format("Nif, node id: %g", params.node_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)

dev_handler:call("nif", params.node_id)
