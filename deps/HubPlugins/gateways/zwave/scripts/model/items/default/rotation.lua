local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.rotation,
    value_type = item_value_type.frequency,
    value = {
        value = 0,
        scale = "revolutions_per_minute",
    },
    has_getter = true,
    has_setter = false
}
