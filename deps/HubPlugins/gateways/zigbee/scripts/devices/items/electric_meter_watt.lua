local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.electric_meter_watt,
    value_type = "power",
    value = {
        value = 0,
        scale = "watt",
    },
    has_getter = true,
    has_setter = false
}
