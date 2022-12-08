local item_names = require("HUB:zwave/scripts/resources/item_names")
local shutter_states_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_states")

local item = {
    name = item_names.shutter_state,
    value_type = "token",
    enum = {},
    has_getter = true,
    has_setter = false,
    value = shutter_states_enum.idle
}

for _, k in pairs(shutter_states_enum) do
    table.insert(item.enum, k)
end

return item
