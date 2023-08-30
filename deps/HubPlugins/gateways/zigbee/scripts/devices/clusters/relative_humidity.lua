local relative_humidity = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function relative_humidity.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    assert(self:find_attribute("MeasuredValue", inclusion_context.cluster))
    local item = self:create_item(inclusion_context, "MeasuredValue", item_names.humidity)
    self:add_items_and_settings_to_child_device(inclusion_context, item.name, { item })
end

return relative_humidity
