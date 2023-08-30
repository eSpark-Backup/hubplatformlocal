return {
    check_all_nodes = false,
    items_old_names = {},
    clusters_to_check = {},
    storage_item_deleter = function(item)
        local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
        storage_helper:call("remove_item_mapping", item.id)
    end,
    should_use_old_item_value = function(item, descriptors)
        local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
        if item.name ~= item_names.user_codes then
            return false
        end

        for _, desc in ipairs(descriptors) do
            if desc.cluster_name == "ias_ace" then
                return true
            end
        end

        return false
    end
}
