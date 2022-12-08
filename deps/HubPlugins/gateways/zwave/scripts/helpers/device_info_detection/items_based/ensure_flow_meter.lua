local items = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

if items[item_names.water_meter_volume] ~= nil or
    items[item_names.water_meter_pulse] ~= nil then
    return {
        name = "Water Flow Meter",
        category = "flow_meter"
    } 
end

if items[item_names.gas_meter_volume] ~= nil or
    items[item_names.gas_meter_pulse] ~= nil then
    return {
        name = "Gas Flow Meter",
        category = "flow_meter"
    } 
end

