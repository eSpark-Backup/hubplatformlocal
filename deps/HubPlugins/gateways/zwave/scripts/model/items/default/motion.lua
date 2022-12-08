local item_names = require("HUB:zwave/scripts/resources/item_names")

local item = {
    name = item_names.motion,
    value_type = "bool",
    value = false,
    has_getter = true,
    has_setter = false
}

return item
