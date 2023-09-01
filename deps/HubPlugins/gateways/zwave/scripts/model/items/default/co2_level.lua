local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.co2_level,
    value_type = item_value_type.substance_amount,
    value = {
        value = 0,
        scale = "parts_per_million",
    },
    has_getter = true,
    has_setter = false
}
