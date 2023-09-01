local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.bed_activity,
    value_type = "int",
    value = 0,
    has_getter = true,
    has_setter = false
}
