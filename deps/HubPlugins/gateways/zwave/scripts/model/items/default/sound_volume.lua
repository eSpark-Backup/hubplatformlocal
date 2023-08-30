local item_names = require("HUB:zwave/scripts/resources/item_names")

return {
    name = item_names.sound_volume,
    value_type = "int",
    value = 100,
    has_getter = true,
    has_setter = true,
    value_min = 0,
    value_max = 100
}
