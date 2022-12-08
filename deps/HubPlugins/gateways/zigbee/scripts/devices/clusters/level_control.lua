local cls_level_control = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function cls_level_control.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    for _, attribute in ipairs(inclusion_context.cluster.attributes) do
        if attribute.name == "CurrentLevel" then
            self:add_items_and_settings(
                inclusion_context,
                {
                    self:create_item(inclusion_context, "CurrentLevel", item_names.dimmer),
                    self:create_item(inclusion_context, "CurrentLevel", item_names.dimmer_up),
                    self:create_item(inclusion_context, "CurrentLevel", item_names.dimmer_down),
                    self:create_item(inclusion_context, "CurrentLevel", item_names.dimmer_stop)
                })
        end
    end
end

return cls_level_control
