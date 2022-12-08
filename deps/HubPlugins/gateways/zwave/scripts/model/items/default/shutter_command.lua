local item_names = require("HUB:zwave/scripts/resources/item_names")
local shutter_commands_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_commands")

local item = {
    name = item_names.shutter_command,
    value_type = "token",
    enum = {},
    has_getter = false,
    has_setter = true,
    value = shutter_commands_enum.off
}

for _, k in pairs(shutter_commands_enum) do
    table.insert(item.enum, k)
end

return item
