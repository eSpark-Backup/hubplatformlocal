local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.weight,
    value_type = "mass",
    value = {
        value = 0,
        scale = "kilo_gram",
    },
    has_getter = true,
    has_setter = false
}
