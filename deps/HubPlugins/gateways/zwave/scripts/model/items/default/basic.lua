local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.basic,
    value_type = "bool",
    value = false,
    has_getter = true,
    has_setter = true
}
