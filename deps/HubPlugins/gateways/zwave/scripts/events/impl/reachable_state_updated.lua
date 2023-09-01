local params = ...

print( string.format("Reachable state was updated, node id: %g, new state: %s", params.node_id, tostring(params.value)))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
-- TODO: Save node id as number and don't convert to string
dev_handler:call("update_reachable_state", tostring(math.floor(params.node_id)), params.value)
