local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local co_alarm_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/co_alarm_event")()

local item = {
    name = item_names.co_alarm,
    value_type = "token",
    enum = {
        co_alarm_enum.unknown,
        co_alarm_enum.no_co,
        co_alarm_enum.co_detected },
    value = co_alarm_enum.unknown,
    has_getter = true,
    has_setter = false
}

return item
