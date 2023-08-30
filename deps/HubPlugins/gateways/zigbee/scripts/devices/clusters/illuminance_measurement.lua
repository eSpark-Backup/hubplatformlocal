local illuminance_measurement = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function illuminance_measurement.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    if not self:find_attribute("MeasuredValue", inclusion_context.cluster) then return end

    local item = self:create_item(inclusion_context, "MeasuredValue", item_names.lux)
    self:add_items_and_settings_to_child_device(inclusion_context, item.name, { item })
end

return illuminance_measurement
