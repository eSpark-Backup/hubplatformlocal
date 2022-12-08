local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.acceleration_x_axis,
    value_type = item_value_type.acceleration,
    value = {
        value = 0,
        scale = "meter_per_square_second",
    },
    has_getter = true,
    has_setter = false
}
