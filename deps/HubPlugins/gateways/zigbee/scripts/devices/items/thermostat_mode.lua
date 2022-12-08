local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local thermostat_mode_enum = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/thermostat_mode")

local item = {
    name = item_names.thermostat_mode,
    value_type = "token",
    enum = {},
    has_getter = true,
    has_setter = true,
    value = thermostat_mode_enum.off
}

for _, k in pairs(thermostat_mode_enum) do
    table.insert(item.enum, k)
end

return item
