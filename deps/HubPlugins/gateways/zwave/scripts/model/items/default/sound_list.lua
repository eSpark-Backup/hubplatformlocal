local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_value_type = require("HUB:zwave/scripts/resources/item_value_type")

return {
    name = item_names.sound_list,
    value_type = "dictionary." .. item_value_type.sound_info,
    value = {},
    has_getter = true,
    has_setter = false
}
