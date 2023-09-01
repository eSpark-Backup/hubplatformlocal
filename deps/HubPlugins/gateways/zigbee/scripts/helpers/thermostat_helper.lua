local thermostat_op_state_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_operating_state")
local thermostat_mode_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_mode")

local helper = {}

helper.min_max_attributes = {
    OccupiedCoolingSetpoint = { min = "MinCoolSetpointLimit", max = "MaxCoolSetpointLimit" },
    OccupiedHeatingSetpoint = { min = "MinHeatSetpointLimit", max = "MaxHeatSetpointLimit" },
}

function helper.mode_to_item_value(attr_value)
    if thermostat_mode_enum[attr_value] then
        return attr_value
    end
end

local op_state_to_item_value = {
    cool = thermostat_op_state_enum.cooling,
    heat = thermostat_op_state_enum.heating,
    fan = thermostat_op_state_enum.fan_only,
    heat_2nd_stage = thermostat_op_state_enum.second_stage_heating,
    cool_2nd_stage = thermostat_op_state_enum.second_stage_cooling,
    fan_2nd_stage = thermostat_op_state_enum.second_stage_fan,
    fan_3rd_stage = thermostat_op_state_enum.third_stage_fan
}

local op_state_priority = {
    [thermostat_op_state_enum.heating] = 1,
    [thermostat_op_state_enum.cooling] = 2,
    [thermostat_op_state_enum.second_stage_heating] = 3,
    [thermostat_op_state_enum.second_stage_cooling] = 4,
    [thermostat_op_state_enum.fan_only] = 5,
    [thermostat_op_state_enum.second_stage_fan] = 6,
    [thermostat_op_state_enum.third_stage_fan] = 7
}

function helper.op_state_to_item_value(attr_value)
    if not attr_value then
        return thermostat_op_state_enum.idle
    end
    
    local result
    for _, state in ipairs(attr_value) do
        local item_value = op_state_to_item_value[state]
        assert(item_value, "Unsupposted thermostat operating state: " .. state)
        if not result or op_state_priority[item_value] < op_state_priority[result] then
            result = item_value
        end
    end
    
    return result
end

return helper
