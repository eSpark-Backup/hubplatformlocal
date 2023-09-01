local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.electrical_conductivity,
    value_type = item_value_type.electrical_conductivity,
    value = {
        value = 0,
        scale = "siemens_per_meter",
    },
    has_getter = true,
    has_setter = false
}
