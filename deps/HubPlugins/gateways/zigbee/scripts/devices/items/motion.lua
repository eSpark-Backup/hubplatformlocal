local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")

local item = {
    name = item_names.motion,
    value_type = "bool",
    value = false,
    has_getter = true,
    has_setter = false
}

return item
