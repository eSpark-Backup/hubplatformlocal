local items = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if items[item_names.rgb_color] ~= nil then
    return {
        name = "RGB Light",
        category = "dimmable_light",
        subcategory = "dimmable_colored"
    } 
end
