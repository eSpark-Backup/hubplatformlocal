local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.tank_capacity,
    value_type = "volume",
    value = {
        value = 0,
        scale = "liter",
    },
    has_getter = true,
    has_setter = false
}
