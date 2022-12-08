local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local item_value_type = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_value_type")

return {
    name = item_names.user_lock_operation,
    value_type = item_value_type.user_lock_operation,
    value = {
        userId = -1,
        action = "unknown"
    },
    has_getter = true,
    has_setter = false
}
