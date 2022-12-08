local self, cc_setpoint, thermostat_mode, setpoint_type_setting_value = ...

local setpoint_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_setpoint")

if setpoint_type_setting_value and setpoint_type_setting_value ~= "default" and setpoint_enum.token_to_value[setpoint_type_setting_value] then
    local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
    local _, found_setpoint = find_if(cc_setpoint.setpoints, function(setpoint) 
        return setpoint.type == setpoint_enum.token_to_value[setpoint_type_setting_value] 
    end)

    return found_setpoint and { found_setpoint }
elseif thermostat_mode then
    return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_setpoints")(cc_setpoint, thermostat_mode)
end

