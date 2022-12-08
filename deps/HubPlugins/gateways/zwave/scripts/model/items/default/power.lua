local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.power,
    value_type = "power",
    value = {
        value = 0,
        scale = "watt",
    },
    has_getter = true,
    has_setter = false
}
