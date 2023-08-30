local handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/default/handler")()

function handler.unknown_attribute_value_updated(
    self, node, endpoint_id, cluster_name, cluster_id, attribute_name, attribute_id, value)
    if cluster_name ~= "occupancy_sensing" or attribute_name ~= "Occupancy" then return end

    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local item_id = handler:find_item_id(
        item_names.motion,
        node.id,
        endpoint_id,
        cluster_name,
        attribute_name)

    if not item_id then return end

    if tonumber(value) ~= 1 then return end

    handler:update_value_in_core(item_id, true)
    handler:update_value_in_core(item_id, false)
end

function handler.to_illuminance(self, value)
    return { value = value, scale = "lux" }
end

return handler
