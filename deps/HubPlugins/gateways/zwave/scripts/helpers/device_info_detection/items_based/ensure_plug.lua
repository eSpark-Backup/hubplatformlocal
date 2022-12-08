local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.switch] ~= nil then
    return {
        name = "Plug",
        category = "switch",
        subcategory = "interior_plugin"
    } 
end
