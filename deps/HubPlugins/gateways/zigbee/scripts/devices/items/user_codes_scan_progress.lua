local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

return {
    name = item_names.user_codes_scan_progress,
    value_type = "int",
    has_getter = true,
    has_setter = true,
    value = 100,
    value_min = 0,
    value_max = 100
}
