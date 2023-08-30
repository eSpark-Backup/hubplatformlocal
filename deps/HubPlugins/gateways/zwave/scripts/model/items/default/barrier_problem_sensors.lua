local item_names = require("HUB:zwave/scripts/resources/item_names")
local barier_problem_sensors_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_problem_sensors")

local item = {
    name = item_names.barrier_problem_sensors,
    value_type = "dictionary.token",
    enum = {},
    value = {},
    has_getter = true,
    has_setter = false
}

for _, k in pairs(barier_problem_sensors_enum) do
    table.insert(item.enum, k)
end

return item
