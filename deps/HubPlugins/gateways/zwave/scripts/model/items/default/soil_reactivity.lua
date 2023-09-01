local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.soil_reactivity,
    value_type = item_value_type.acidity,
    value = {
        value = 0,
        scale = "potential_of_hydrogen",
    },
    has_getter = true,
    has_setter = false
}
