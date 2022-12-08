local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.sound_select,
    value_type = "int",
    value = 1,
    has_getter = true,
    has_setter = true,
    value_min = 1,
}
