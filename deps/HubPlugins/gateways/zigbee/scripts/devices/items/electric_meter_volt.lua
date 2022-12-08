local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.electric_meter_volt,
    value_type = "electric_potential",
    value = {
        value = 0,
        scale = "volt",
    },
    has_getter = true,
    has_setter = false
}
