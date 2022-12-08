local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.body_mass_index,
    value_type = item_value_type.float,
    value = 0,
    has_getter = true,
    has_setter = false
}
