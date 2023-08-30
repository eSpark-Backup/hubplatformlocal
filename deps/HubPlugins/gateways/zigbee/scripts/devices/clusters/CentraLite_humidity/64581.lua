local cls_humidity_measurement = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_humidity_measurement.process_cluster_mfg_specific(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    assert(self:find_attribute_by_id(0x0000, inclusion_context.cluster))

    local item = self:create_mfg_sprcific_item(inclusion_context, 0x0000, item_names.humidity)
    self:add_items_and_settings_to_child_device(inclusion_context, item.name, { item })
end

return cls_humidity_measurement
