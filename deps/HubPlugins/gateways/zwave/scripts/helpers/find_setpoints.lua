local cc_setpoint, mode = ...

if not cc_setpoint or not cc_setpoint.setpoints then
    return
end

local item_names = require("HUB:zwave/scripts/resources/item_names")

local enum_setpoint = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")

local possible_setpoints = hubplugins.weak_require("HUB:zwave/scripts/resources/thermostat_setpoints")[mode]
if possible_setpoints == nil then
    return {}
end

local result = {}
for _, setpoint in ipairs(cc_setpoint.setpoints) do
    local setpoint_name = enum_setpoint.value_to_token[setpoint.type]

    if possible_setpoints[setpoint_name] ~= nil then
        table.insert(result, setpoint)
    end
end

return result
