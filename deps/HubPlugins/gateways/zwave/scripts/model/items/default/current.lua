local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.current,
    value_type = "electric_current",
    value = {
        value = 0,
        scale = "ampere",
    },
    has_getter = true,
    has_setter = false
}
