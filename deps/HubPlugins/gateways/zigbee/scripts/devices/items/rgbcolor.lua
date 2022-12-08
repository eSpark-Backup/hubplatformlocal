local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.rgb_color,
    value_type = "rgb",
    value = {},
    has_getter = true,
    has_setter = true
}
