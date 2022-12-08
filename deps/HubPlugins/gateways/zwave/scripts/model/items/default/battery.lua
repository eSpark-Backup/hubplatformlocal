local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.battery,
    value_type = "int",
    value = 100,
    has_getter = true,
    has_setter = false
}
