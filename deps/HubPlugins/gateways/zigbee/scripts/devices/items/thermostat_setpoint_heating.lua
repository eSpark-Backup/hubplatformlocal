local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.thermostat_setpoint_heating,
    value_type = "temperature",
    value = {
        value = 0,
        scale = "celsius",
    },
    has_getter = true,
    has_setter = true,
    show = false
}
