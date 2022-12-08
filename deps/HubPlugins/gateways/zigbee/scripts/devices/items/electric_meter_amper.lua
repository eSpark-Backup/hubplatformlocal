local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.electric_meter_amper,
    value_type = "electric_current",
    value = {
        value = 0,
        scale = "ampere",
    },
    has_getter = true,
    has_setter = false
}
