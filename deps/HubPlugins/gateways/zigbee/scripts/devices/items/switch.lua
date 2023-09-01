local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local types = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_value_type")

return {
    name = item_names.switch,
    value_type = types.bool,
    value = false,
    has_getter = true,
    has_setter = true
}
