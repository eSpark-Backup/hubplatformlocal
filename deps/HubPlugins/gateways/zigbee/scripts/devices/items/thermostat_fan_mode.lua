local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local fan_mode_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_fan_mode")

return {
    name = item_names.thermostat_fan_mode,
    value_type = "token",
    enum = {},
    -- value = first available token from enum
    has_getter = true,
    has_setter = true
}
