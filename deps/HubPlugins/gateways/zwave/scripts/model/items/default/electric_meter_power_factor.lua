local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.electric_meter_factor,
    value_type = item_value_type.general_purpose,
    value = {
        value = 0,
        scale = "float",
    },
    has_getter = true,
    has_setter = false
}
