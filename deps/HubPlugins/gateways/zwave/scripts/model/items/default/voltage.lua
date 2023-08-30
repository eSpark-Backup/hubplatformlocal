local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.voltage,
    value_type = "electric_potential",
    value = {
        value = 0,
        scale = "volt",
    },
    has_getter = true,
    has_setter = false
}
