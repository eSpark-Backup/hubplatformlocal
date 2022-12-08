local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.relative_modulation_level,
    value_type = item_value_type.general_purpose,
    value = {
        value = 0,
        scale = "percent",
    },
    has_getter = true,
    has_setter = false
}
