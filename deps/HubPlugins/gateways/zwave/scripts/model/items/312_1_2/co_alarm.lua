local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/co_alarm")()
local co_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/co_alarm_event")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")(
    item.enum,
    co_alarm_enum.no_co,
    co_alarm_enum.co_detected
)

return item
