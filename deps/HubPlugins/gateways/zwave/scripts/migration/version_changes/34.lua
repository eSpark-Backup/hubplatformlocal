local info = hubplugins.weak_loadfile("HUB:zwave/scripts/migration/version_changes/default")()
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

info.cc_to_check = {
    [class_ids.central_scene] = true,
    [class_ids.scene_activation] = true,
}

local default_items_comparator = info.items_comparator
info.items_comparator = function(storage, old_item, new_item, items_old_names)
    if old_item.name ~= "button_state" then
        return default_items_comparator(storage, old_item, new_item, items_old_names)
    end
    
    local old_descriptors = storage:call("find_item_descriptors", old_item.id)
    local new_descriptor = new_item.descriptor
    for _, old_descriptor in ipairs(old_descriptors) do
        if (new_descriptor.class_id == old_descriptor.class_id)
        and (new_descriptor.item_name == old_descriptor.item_name or new_descriptor.item_name == items_old_names[old_descriptor.item_name])
        and (math.floor(new_descriptor.tag) == math.floor(old_descriptor.tag)) then
            return true
        end
    end
    
    return false
end

return info
