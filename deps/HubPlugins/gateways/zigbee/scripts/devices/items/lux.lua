local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.lux,
    value_type = "illuminance",
    value = {
        value = 0,
        scale = "lux",
    },
    has_getter = true,
    has_setter = false
}
