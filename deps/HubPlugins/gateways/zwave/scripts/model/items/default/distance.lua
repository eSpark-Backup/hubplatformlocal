local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.distance,
    value_type = item_value_type.length,
    value = {
        value = 0,
        scale = "meter",
    },
    has_getter = true,
    has_setter = false
}
