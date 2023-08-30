local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.electrical_resistance,
    value_type = item_value_type.electrical_resistance,
    value = {
        value = 0,
        scale = "ohm_meter",
    },
    has_getter = true,
    has_setter = false
}
