local item_names = require("HUB:zwave/scripts/resources/item_names")
local barrier_unattended_operation_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_unattended_operation")

local item = {
    name = item_names.barrier_unattended_operation,
    value_type = "token",
    enum = { barrier_unattended_operation_enum.unattended_operation_enabled },
    value = barrier_unattended_operation_enum.unknown,
    has_getter = true,
    has_setter = false
}

return item
