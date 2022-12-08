local item_names = require("HUB:zwave/scripts/resources/item_names")
local barrier_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_event")

local item = {
    name = item_names.barrier_state,
    value_type = "token",
    enum = {},
    value = barrier_enum.operating_unknown,
    has_getter = false,
    has_setter = true
}

for _, k in pairs(barrier_enum) do
    table.insert(item.enum, k)
end

return item
