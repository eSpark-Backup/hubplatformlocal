local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.ultraviolet,
    value_type = "ultraviolet",
    value = {
        value = 0,
        scale = "uv_index",
    },
    has_getter = true,
    has_setter = false
}
