local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.goto_favorite,
    value_type = "bool",
    value = false,
    has_getter = false,
    has_setter = true
}