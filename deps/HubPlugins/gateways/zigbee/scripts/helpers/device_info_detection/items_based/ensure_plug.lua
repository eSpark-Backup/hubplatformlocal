local items = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if items[item_names.switch] ~= nil then
    return {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin"
    } 
end
