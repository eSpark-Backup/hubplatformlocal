local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.radon_concentration,
    value_type = item_value_type.radon_concentration,
    value = {
        value = 0,
        scale = "becquerel_per_cubic_meter",
    },
    has_getter = true,
    has_setter = false
}
