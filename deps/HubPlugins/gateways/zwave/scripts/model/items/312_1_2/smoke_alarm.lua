local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/smoke_alarm")()
local smoke_alarm_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/smoke_alarm_event")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")(
    item.enum,
    smoke_alarm_enum.no_smoke,
    smoke_alarm_enum.smoke_detected
)

return item
