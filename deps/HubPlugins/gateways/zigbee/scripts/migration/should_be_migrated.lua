local zigbee_node, changes = ...

local storage = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local device_id = storage:call("find_device_id", zigbee_node.id)
if not device_id then
    return true
end

if changes.check_all_nodes then
    return true
end

if changes.devices_to_check then
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/create_device_type_id")(zigbee_node.manufacturer_id, zigbee_node.model_id)
    if changes.devices_to_check[dev_handler_id] then
        return true
    end
end

if not next(changes.clusters_to_check) then
    return false
end

for _, ep in ipairs(zigbee_node.endpoints) do
    for _, cluster in ipairs(ep.clusters) do
        if changes.clusters_to_check[cluster.name] then
            return true
        end
    end
end

return false
