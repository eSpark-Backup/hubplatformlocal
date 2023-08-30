local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local op_state_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/thermostat_operating_state")()

local item = {
    name = item_names.thermostat_operating_state,
    value_type = "token",
    enum = {},
    value = op_state_enum.idle,
    has_getter = true,
    has_setter = false
}

for _, k in pairs(op_state_enum) do
    table.insert(item.enum, k)
end

return item
