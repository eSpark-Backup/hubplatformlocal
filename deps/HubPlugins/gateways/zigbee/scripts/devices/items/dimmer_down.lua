local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.dimmer_down,
    value_type = "int",
    value = 0,
    has_getter = false,
    has_setter = true
}
