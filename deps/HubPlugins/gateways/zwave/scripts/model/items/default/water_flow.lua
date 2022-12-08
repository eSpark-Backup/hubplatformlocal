local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.water_flow,
    value_type = item_value_type.volume_flow,
    value = {
        value = 0,
        scale = "liter_per_hour",
    },
    has_getter = true,
    has_setter = false
}
