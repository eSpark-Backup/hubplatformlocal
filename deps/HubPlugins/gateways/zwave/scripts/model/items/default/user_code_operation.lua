local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.user_code_operation,
    value_type = "token",
    enum = { "no_operation" },
    value = "no_operation",
    has_getter = true,
    has_setter = false
}
