local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/thermostat_fan_state")()
local fan_state_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_fan_state")

item.enum = {
    fan_state_enum.value_to_token[0],
    fan_state_enum.value_to_token[1],
}
item.value = fan_state_enum.value_to_token[0]

return item
