local items = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if items[item_names.door_lock] ~= nil then
    return {
        name = "Door Lock",
        category = "door_lock",
        subcategory = ""
    } 
end
