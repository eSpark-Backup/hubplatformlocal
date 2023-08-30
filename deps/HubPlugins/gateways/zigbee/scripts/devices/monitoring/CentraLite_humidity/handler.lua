local handler = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/monitoring/default/handler")()

function handler.mfg_specific_attribute_value_updated(self, node, endpoint_id, cluster_id, attribute)
    if cluster_id ~= 0xFC45 or attribute.id ~= 0x00 then return end

    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
    local mfg_specific_tag_helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/mfg_specific_tag_helper")()

    local new_value = tonumber(attribute.formatted_value)
    if new_value == 0xffff then return end; -- undefined

    local item_id = handler:find_item_id(
        item_names.humidity,
        node.id,
        endpoint_id,
        math.floor(cluster_id),
        math.floor(attribute.id),
        mfg_specific_tag_helper.add_mfg_specific_tag())
    if not item_id then return end

    local min_value = tonumber(handler:find_attribute_by_ids(node, endpoint_id, 0xFC45, 0x0001).formatted_value)
    local max_value = tonumber(handler:find_attribute_by_ids(node, endpoint_id, 0xFC45, 0x0002).formatted_value)

    if min_value == 0xffff or max_value == 0xffff then -- undefined
        handler:update_value_in_core(item_id, handler:to_relative_mesurement(new_value))
    else
        handler:update_value_with_min_max_in_core(
            item_id,
            handler:to_relative_mesurement(new_value),
            handler:to_relative_mesurement(min_value),
            handler:to_relative_mesurement(max_value)
        )
    end
end

return handler
