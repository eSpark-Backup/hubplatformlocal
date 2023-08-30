local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.dimmer,
    value_type = "int",
    has_getter = true,
    has_setter = true,
    value = 0,
    value_min = 0,
    value_max = 100,
}
