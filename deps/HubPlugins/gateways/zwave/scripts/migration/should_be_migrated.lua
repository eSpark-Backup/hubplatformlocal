local zwave_node, changes = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = storage:call("find_device_id", zwave_node.id)
if not device_id then
    return true
end

if changes.check_all_nodes then
    return true
end

if changes.cc_to_check and next(changes.cc_to_check) then
    for _, channel in ipairs(zwave_node.channels) do
        for _, cc in ipairs(channel.classes) do
            if changes.cc_to_check[cc.id] then
                return true
            end
        end
    end
end

if changes.devices_to_check then
    local dev_handler_id = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/create_device_type_id")(zwave_node.manufacturer_id, zwave_node.product_type, zwave_node.product_id)
    if changes.devices_to_check[dev_handler_id] then
        return true
    end
end

return false
