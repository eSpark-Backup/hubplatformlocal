local item_names = require("HUB:zwave/scripts/resources/item_names")
local barrier_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_event")

local item = {
    name = item_names.barrier,
    value_type = "token",
    enum = { barrier_enum.barrier_closed, barrier_enum.barrier_opened },
    value = barrier_enum.barrier_closed,
    has_getter = false,
    has_setter = true
}

return item
