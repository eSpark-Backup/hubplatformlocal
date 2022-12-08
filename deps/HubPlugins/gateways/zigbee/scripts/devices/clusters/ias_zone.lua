local ias_zone = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function ias_zone.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
    local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/ias_zone_helper")()

    if not self:find_attribute("ZoneType", inclusion_context.cluster) then
        return
    end

    local items = {}

    -- add tamper
    local device_node_data = inclusion_context.tree_model:call("get_data", inclusion_context.current_processing_node)
    if device_node_data.info.has_ias_zone_tamper then
        table.insert(items, self:create_item(inclusion_context, "ZoneStatus", item_names.tampering_cover_alarm))
    end

    -- add sensor
    local zone_type_attr = self:find_attribute("ZoneType", inclusion_context.cluster)
    assert(zone_type_attr)
    if not zone_type_attr.formatted_value then return end

    local callback = function(alarm_type)
        local tag = alarm_type
        local item_name = helper.zone_type_to_item_name(zone_type_attr.formatted_value.zone_type, tag)

        if item_name then
            local item = self:create_item(inclusion_context, "ZoneStatus", item_name, tag)
            local additional_states = helper.get_additional_states(zone_type_attr.formatted_value.zone_type)
            if additional_states then
                for _, state in ipairs(additional_states) do
                    table.insert(item.enum, state)
                end
            end

            table.insert(items, item)
            if helper.add_to_child_device(item_name) then
                self:add_items_and_settings_to_child_device(inclusion_context, item.name, items)
            else
                self:add_items_and_settings(inclusion_context, items)
            end

            items = {}
            return true
        else
            return false
        end
    end

    if not callback() then
        callback("alarm_1")
        callback("alarm_2")
    end

    if next(items) then
        self:add_items_and_settings(inclusion_context, items)
    end
end

return ias_zone
