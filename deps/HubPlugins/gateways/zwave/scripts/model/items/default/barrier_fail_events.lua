local item_names = require("HUB:zwave/scripts/resources/item_names")
local barrier_fail_events_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/barrier_fail_events")

local item = {
    name = item_names.barrier_fail_events,
    value_type = "token",
    enum = { barrier_fail_events_enum.no_barrier_fails },
    value = barrier_fail_events_enum.no_barrier_fails,
    has_getter = true,
    has_setter = false
}

return item
