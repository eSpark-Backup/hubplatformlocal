local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.seismic_intensity,
    value_type = item_value_type.seismic_intensity,
    value = {
        value = 0,
        scale = "mercalli",
    },
    has_getter = true,
    has_setter = false
}
