local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.co_level,
    value_type = item_value_type.substance_amount,
    value = {
        value = 0,
        scale = "mole_per_cubic_meter",
    },
    has_getter = true,
    has_setter = false
}
