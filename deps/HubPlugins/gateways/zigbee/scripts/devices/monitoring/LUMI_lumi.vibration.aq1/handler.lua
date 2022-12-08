local handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/default/handler")()

function handler.unknown_attribute_value_updated(
    self, node, endpoint_id, cluster_name, cluster_id, attribute_name, attribute_id, value)
    if cluster_name ~= "door_lock" then return end

    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    if attribute_id == 0x0503 then
        local item_id = handler:find_item_id(
            item_names.bed_activity,
            node.id,
            endpoint_id,
            cluster_name,
            "" )

        if item_id then
            handler:update_value_in_core(item_id, tonumber(value))
        end

    elseif attribute_id == 0x0055 then
        local item_id = handler:find_item_id(
            item_names.activity,
            node.id,
            endpoint_id,
            cluster_name,
            "" )

        local enum_value = self:to_activity(value)
        if item_id and enum_value then
             handler:update_value_in_core(item_id, enum_value)
             local activity_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/activity")()
             handler:update_value_in_core_with_timeout(node.id, item_id, activity_enum.no_activity, 60000)
        end
    end
end

function handler.to_activity(self, value)
    local activity_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/activity")()

    local mapping = {
        [1] = activity_enum.shake,
        [2] = activity_enum.tilt,
        [3] = activity_enum.drop
    }

    return mapping[tonumber(value)]
end

return handler
