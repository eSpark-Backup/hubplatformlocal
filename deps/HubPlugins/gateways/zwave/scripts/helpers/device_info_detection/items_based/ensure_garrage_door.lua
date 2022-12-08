local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.barrier] ~= nil or items[item_names.barrier_state] ~= nil then
    return {
        name = "Garrage Door",
        category = "garage_door"
    } 
end
