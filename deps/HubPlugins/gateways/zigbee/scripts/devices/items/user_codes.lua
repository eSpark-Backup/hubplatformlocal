local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local item_value_type = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_value_type")

return {
    name = item_names.user_codes,
    value_type = "dictionary." .. item_value_type.usercode,
    value = {},
    has_getter = true,
    has_setter = true
}
