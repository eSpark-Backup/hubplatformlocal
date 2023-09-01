local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.water_pressure,
    value_type = item_value_type.pressure,
    value = {
        value = 0,
        scale = "kilo_pascal",
    },
    has_getter = true,
    has_setter = false
}
