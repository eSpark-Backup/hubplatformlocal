local params = ...

local core = assert(require("core"), "Fail to load the 'core' functionality")
if core.notify_process_started then
    core.notify_process_started("removing_device")
end

print("LUA: Node removed " .. params.node_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/get_device_handler_exclusion")(params.manufacturer_id, params.model_id)
dev_handler:remove_node(params.node_id)

if core.notify_process_stopped then
    core.notify_process_stopped("removing_device", "success")
end
