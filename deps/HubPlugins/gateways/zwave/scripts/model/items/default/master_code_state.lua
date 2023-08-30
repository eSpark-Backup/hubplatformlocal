local item_names = require("HUB:zwave/scripts/resources/item_names")

local master_code_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/master_code_state")

local item = {
    name = item_names.master_code_state,
    value_type = "token",
    enum = { },
    value = master_code_enum.unknown,
    has_getter = true,
    has_setter = true
}

for _, value in pairs(master_code_enum) do
    table.insert(item.enum, value)
end

return item
