local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.respiratory_rate,
    value_type = item_value_type.frequency,
    value = {
        value = 0,
        scale = "breaths_per_minute",
    },
    has_getter = true,
    has_setter = false
}
