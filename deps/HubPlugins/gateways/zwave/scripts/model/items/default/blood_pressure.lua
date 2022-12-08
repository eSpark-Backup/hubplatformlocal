local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.blood_pressure,
    value_type = item_value_type.blood_pressure,
    value = {
        value = 0,
        scale = "systolic",
    },
    has_getter = true,
    has_setter = false
}
