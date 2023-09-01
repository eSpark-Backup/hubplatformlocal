local cls_6_switch = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_6_switch.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    for _, attribute in ipairs(inclusion_context.cluster.attributes) do
        if attribute.name == "OnOff" then
            local item = self:create_item(inclusion_context, "OnOff", item_names.switch)
            self:add_items_and_settings(inclusion_context, { item })
        end
    end
end

return cls_6_switch
