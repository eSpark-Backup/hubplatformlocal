local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.moisture,
    value_type = item_value_type.moisture,
    value = {
        value = 0,
        scale = "percent",
    },
    has_getter = true,
    has_setter = false
}
