local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.door_lock] ~= nil then
    return {
        name = "Door Lock",
        category = "door_lock"
    }
end
