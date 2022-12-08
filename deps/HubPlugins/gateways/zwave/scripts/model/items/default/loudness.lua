local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.loudness,
    value_type = item_value_type.loudness,
    value = {
        value = 0,
        scale = "decibel",
    },
    has_getter = true,
    has_setter = false
}
