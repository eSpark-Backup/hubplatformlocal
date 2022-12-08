local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.rgb_color,
    value_type = "rgb",
    value = {}, -- '0' for all available colors
    has_getter = true,
    has_setter = true
}
