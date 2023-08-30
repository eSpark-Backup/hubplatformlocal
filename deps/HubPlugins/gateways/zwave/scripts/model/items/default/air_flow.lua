local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.air_flow,
    value_type = "volume_flow",
    value = {
        value = 0,
        scale = "cubic_meter_per_hour",
    },
    has_getter = true,
    has_setter = false
}
