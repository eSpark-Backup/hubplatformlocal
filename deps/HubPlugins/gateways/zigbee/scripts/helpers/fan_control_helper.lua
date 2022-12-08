local fan_mode_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

local helper = {}

helper.attr_value_to_enum = {
    off = fan_mode_enum.off,
    low = fan_mode_enum.low,
    medium = fan_mode_enum.medium,
    high = fan_mode_enum.high,
    on = fan_mode_enum.on,
    auto = fan_mode_enum.auto,
}

helper.enum_to_attr_value = {}

for attr_value, enum in pairs(helper.attr_value_to_enum) do
    helper.enum_to_attr_value[enum] = attr_value
end

return helper
