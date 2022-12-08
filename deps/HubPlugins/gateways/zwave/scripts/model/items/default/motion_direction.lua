local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.motion_direction,
    value_type = "direction",
    value = {
        value = 0,
        scale = "no_direction",
    },
    has_getter = true,
    has_setter = false
}
