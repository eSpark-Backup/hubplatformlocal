local occupancy_sensing = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function occupancy_sensing.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local item = self:create_item(inclusion_context, "Occupancy", item_names.motion)
    self:add_items_and_settings_to_child_device(inclusion_context, item.name, { item })
end

return occupancy_sensing
