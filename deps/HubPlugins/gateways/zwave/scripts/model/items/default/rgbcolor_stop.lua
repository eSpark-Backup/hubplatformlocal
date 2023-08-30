local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.rgb_color_stop,
    value_type = "token",
    value = "all",
    enum = { "all" },
    has_getter = false,
    has_setter = true
}
