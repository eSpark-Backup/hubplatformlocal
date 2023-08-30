local door_lock = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/clusters/base")()

function door_lock.process_cluster(self, inclusion_context)
    local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

    local items = {
        self:create_item(inclusion_context, "", item_names.activity),
        self:create_item(inclusion_context, "", item_names.bed_activity)
    }

    self:add_items_and_settings_to_child_device(inclusion_context, item_names.activity, items)
end

return door_lock
