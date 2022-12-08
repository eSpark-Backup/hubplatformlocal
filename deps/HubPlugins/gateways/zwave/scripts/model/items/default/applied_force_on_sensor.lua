local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.applied_force_on_sensor,
    value_type = item_value_type.force,
    value = {
        value = 0,
        scale = "newton",
    },
    has_getter = true,
    has_setter = false
}
