local handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/default/handler")()

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local base_attr_updated = handler.attribute_value_updated
function handler.attribute_value_updated(self, node, endpoint_id, cluster_name, attribute)
    if cluster_name == "level_control" and attribute.name == "CurrentLevel" then
        local item_id = handler:find_item_id(item_names.switch, node.id, endpoint_id, cluster_name, attribute.name)
        if item_id then
            local new_value = attribute.formatted_value ~= 0
            self:update_value_in_core(item_id, new_value)
        end
    else
        base_attr_updated(self, node, endpoint_id, cluster_name, attribute)
    end
end

return handler
