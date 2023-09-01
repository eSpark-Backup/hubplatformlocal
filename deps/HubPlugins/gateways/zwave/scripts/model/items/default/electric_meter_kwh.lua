local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.electric_meter_kwh,
    value_type = item_value_type.amount_of_useful_energy,
    value = {
        value = 0,
        scale = "kilo_watt_hour",
    },
    has_getter = true,
    has_setter = false
}
