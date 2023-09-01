local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.button_state] ~= nil then
    return {
        name = "Scene Controller",
        category = "scene_controller",
        subcategory = "generic"
    } 
end
