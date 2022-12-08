local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.domestic_hot_water_temperature,
    value_type = item_value_type.temperature,
    value = {
        value = 0,
        scale = "celsius",
    },
    has_getter = true,
    has_setter = false
}
