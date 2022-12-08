local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.thermostat_fan_mode] ~= nil
or items[item_names.thermostat_fan_state] ~= nil
or items[item_names.thermostat_mode] ~= nil
or items[item_names.thermostat_setpoint] ~= nil
or items[item_names.thermostat_operating_state] ~= nil then
    return {
        name = "Thermostat",
        category = "hvac",
        subcategory = "hvac"
    } 
end
