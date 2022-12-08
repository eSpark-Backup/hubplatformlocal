local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.temp,
    value_type = "temperature",
    value = {
        value = 0,
        scale = "celsius",
    },
    value_min = {
        value = -85,
        scale = "celsius",
    },
    value_max = {
        value = 100,
        scale = "celsius",
    },
    has_getter = true,
    has_setter = false
}
