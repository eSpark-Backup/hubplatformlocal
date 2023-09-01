local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local water_leak_alarm_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/water_leak_alarm_event")()

local item = {
    name = item_names.water_leak_alarm,
    value_type = "token",
    enum = {
        water_leak_alarm_enum.unknown,
        water_leak_alarm_enum.no_water_leak,
        water_leak_alarm_enum.water_leak_detected },
    value = water_leak_alarm_enum.unknown,
    has_getter = true,
    has_setter = false
}

return item
