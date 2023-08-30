local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.rain_rate,
    value_type = "precipitation",
    value = {
        value = 0,
        scale = "milli_meter_per_hour",
    },
    has_getter = true,
    has_setter = false
}
