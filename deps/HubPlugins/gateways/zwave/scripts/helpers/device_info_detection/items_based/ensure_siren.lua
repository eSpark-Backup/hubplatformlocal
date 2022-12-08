local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.switch] ~= nil and items[item_names.battery] ~= nil or
   items[item_names.sounding_mode] ~= nil or items[item_names.siren_alarm] ~= nil then
    return {
        name = "Siren",
        category = "siren"
    } 
end
