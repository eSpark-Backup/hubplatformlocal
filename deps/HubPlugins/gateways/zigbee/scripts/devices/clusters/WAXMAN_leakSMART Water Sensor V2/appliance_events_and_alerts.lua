local cls_humidity_measurement = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_humidity_measurement.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
    self:add_items_and_settings_to_child_device(
        inclusion_context,
        item_names.water_leak_alarm,
        {self:create_item(inclusion_context, "", item_names.water_leak_alarm)}
    )
end

return cls_humidity_measurement
