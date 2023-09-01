local items = ...

local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

if items[item_names.thermostat_mode] ~= nil then
    return {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac"
    } 
end
