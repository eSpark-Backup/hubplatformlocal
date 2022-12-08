local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.water_oxidation_reduction_potential,
    value_type = item_value_type.electric_potential,
    value = {
        value = 0,
        scale = "milli_volt",
    },
    has_getter = true,
    has_setter = false
}
