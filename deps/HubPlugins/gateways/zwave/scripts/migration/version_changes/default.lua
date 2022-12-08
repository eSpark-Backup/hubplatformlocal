return {
    check_all_nodes = false,
    items_old_names = {},
    cc_to_check = {},
    items_to_change_value = {},
    storage_item_deleter = function(item, skip_monitoring_cache)
        local default_exclusion_handler = hubplugins.object("default", "HUB:zwave/scripts/model/devices/exclusion")
        default_exclusion_handler:call("remove_storage_item", item, skip_monitoring_cache)
    end,
    storage_setting_deleter = function(setting_id)
        local default_exclusion_handler = hubplugins.object("default", "HUB:zwave/scripts/model/devices/exclusion")
        default_exclusion_handler:call("remove_storage_setting", setting_id)
    end,
    items_comparator = function(storage, old_item, new_item, items_old_names)
        local old_descriptors = storage:call("find_item_descriptors", old_item.id)
        local new_descriptor = new_item.descriptor
        for _, old_descriptor in ipairs(old_descriptors) do
            if (new_descriptor.class_id == old_descriptor.class_id)
            and (new_descriptor.item_name == old_descriptor.item_name or new_descriptor.item_name == items_old_names[old_descriptor.item_name])
            and (tostring(new_descriptor.tag) == tostring(old_descriptor.tag)) then
                return true
            end
        end
        
        return false
    end
}
