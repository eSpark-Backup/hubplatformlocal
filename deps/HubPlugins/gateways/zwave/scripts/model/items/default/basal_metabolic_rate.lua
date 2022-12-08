local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.basal_metabolic_rate,
    value_type = item_value_type.energy,
    value = {
        value = 0,
        scale = "joule",
    },
    has_getter = true,
    has_setter = false
}
