local item_names = require("HUB:zwave/scripts/resources/item_names")
local fan_state_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_fan_state")

local enum = {}
for _, token in pairs(fan_state_enum.value_to_token) do
    table.insert(enum, token)
end

return {
    name = item_names.thermostat_fan_state,
    value_type = "token",
    has_getter = true,
    has_setter = false,
    enum = enum,
    value = fan_state_enum.value_to_token[0--[[idle_off]]],
}
