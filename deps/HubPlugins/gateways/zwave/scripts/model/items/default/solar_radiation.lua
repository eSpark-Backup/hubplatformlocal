local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.solar_radiation,
    value_type = "irradiance",
    value = {
        value = 0,
        scale = "watt_per_square_meter",
    },
    has_getter = true,
    has_setter = false
}
