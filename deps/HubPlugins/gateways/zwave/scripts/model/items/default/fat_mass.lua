local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.fat_mass,
    value_type = item_value_type.mass,
    value = {
        value = 0,
        scale = "kilo_gram",
    },
    has_getter = true,
    has_setter = false
}
