local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.thermostat_setpoint_heating,
    value_type = "temperature",
    value = {
        value = 0,
        scale = "celsius",
    },
    value_min = {
        value = 0,
        scale = "celsius",
    },
    value_max = {
        value = 33,
        scale = "celsius",
    },
    has_getter = true,
    has_setter = true,
    show = false
}
