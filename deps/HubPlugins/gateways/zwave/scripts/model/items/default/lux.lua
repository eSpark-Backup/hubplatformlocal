local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.lux,
    value_type = "illuminance",
    value = {
        value = 0,
        scale = "percent",
    },
    has_getter = true,
    has_setter = false
}
