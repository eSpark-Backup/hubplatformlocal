local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.velocity,
    value_type = "velocity",
    value = {
        value = 0,
        scale = "meter_per_second",
    },
    has_getter = true,
    has_setter = false
}
