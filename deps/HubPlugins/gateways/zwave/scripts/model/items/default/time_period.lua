local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.time_period,
    value_type = item_value_type.time,
    value = {
        value = 0,
        scale = "seconds",
    },
    has_getter = true,
    has_setter = false
}
