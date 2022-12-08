local cls_electrical_measurement = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

function cls_electrical_measurement.process_cluster(self, inclusion_context)
    local helper = hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/electrical_measurement_helper")()
    local items = {}

    for _, attribute in ipairs(inclusion_context.cluster.attributes) do
        local item_name = helper.attribute_to_item_name[attribute.name]
        if item_name then
            table.insert(items, self:create_item(inclusion_context, attribute.name, item_name))
        end
    end

    self:add_items_and_settings(inclusion_context, items)
end

return cls_electrical_measurement
