local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.electric_meter_kvar,
    value_type = item_value_type.reactive_power_instant,
    value = {
        value = 0,
        scale = "kilo_volt_amper_reactive",
    },
    has_getter = true,
    has_setter = false
}
