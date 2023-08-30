local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local smoke_alarm_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/smoke_alarm_event")()

local item = {
    name = item_names.smoke_alarm,
    value_type = "token",
    enum = {
        smoke_alarm_enum.unknown,
        smoke_alarm_enum.no_smoke,
        smoke_alarm_enum.smoke_detected },
    value = smoke_alarm_enum.unknown,
    has_getter = true,
    has_setter = false
}

return item
