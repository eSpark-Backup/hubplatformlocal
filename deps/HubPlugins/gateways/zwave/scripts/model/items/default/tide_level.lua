local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.tide_level,
    value_type = "length",
    value = {
        value = 0,
        scale = "meter",
    },
    has_getter = true,
    has_setter = false
}
