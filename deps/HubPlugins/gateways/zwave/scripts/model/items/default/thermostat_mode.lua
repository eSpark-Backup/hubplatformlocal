local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.thermostat_mode,
    value_type = "token",
    enum = {},
    -- value = first available token from enum
    has_getter = true,
    has_setter = true
}
