local item_names = require("HUB:zwave/scripts/resources/item_names")
local mode_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/thermostat_energy_saving_mode")

local item = {
    name = item_names.thermostat_energy_saving_mode,
    value_type = "token",
    enum = {},
    value = mode_enum.off,
    has_getter = true,
    has_setter = true
}

for _, k in pairs(mode_enum) do
    table.insert(item.enum, k)
end

return item
