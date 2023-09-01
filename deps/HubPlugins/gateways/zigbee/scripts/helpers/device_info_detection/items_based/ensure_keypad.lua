local items = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if items[item_names.zones_status] ~= nil then
    return {
        name = "Keypad",
        category = "keypad",
        subcategory = ""
    } 
end
