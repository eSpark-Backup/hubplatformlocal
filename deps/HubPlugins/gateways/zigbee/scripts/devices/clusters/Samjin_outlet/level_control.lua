local cls_level_control = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/level_control")()

function cls_level_control.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    if self:find_attribute("CurrentLevel", inclusion_context.cluster) then
        local item = self:create_item(inclusion_context, "CurrentLevel", item_names.switch)
        self:add_items_and_settings(inclusion_context, { item })
    end
end

return cls_level_control
