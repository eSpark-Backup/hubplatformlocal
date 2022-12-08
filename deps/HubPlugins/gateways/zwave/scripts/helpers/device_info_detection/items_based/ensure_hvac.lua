local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.heating_meter_energy] ~= nil and
   items[item_names.cooling_meter_energy] ~= nil then
    return {
        name = "HVAC",
        category = "hvac",
        subcategory = "hvac"
    } 
end

if items[item_names.heating_meter_energy] ~= nil then
    return {
        name = "Heater",
        category = "hvac",
        subcategory = "heater"
    } 
end

if items[item_names.cooling_meter_energy] ~= nil then
    return {
        name = "Cooler",
        category = "hvac",
        subcategory = "cooler"
    } 
end
