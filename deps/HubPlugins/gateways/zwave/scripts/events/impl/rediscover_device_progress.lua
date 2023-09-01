local params = ...

print(("Rediscover node %g, progress %s, status %s"):format(params.node_id, tostring(params.progress), params.status ))

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = storage:call("find_device_id", params.node_id)

if device_id then
    local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
    dev_handler:call("handle_node_rediscover_progress", params.node_id, params.progress, params.status, params.error)
elseif params.status == "finished" then
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/run_node_rediscover_migration")(params.node_id)
end
