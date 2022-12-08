
local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
return {
    name = item_names.button_state,
    value_type = "button_state",
    has_getter = true,
    has_setter = false
}
