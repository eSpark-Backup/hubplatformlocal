local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.cooling_meter_energy,
    has_setter = false,
    has_getter = true,
    value_type = item_value_type.energy,
    value = {
        value = 0,
        scale = "kilo_watt_hour"
    }
}
