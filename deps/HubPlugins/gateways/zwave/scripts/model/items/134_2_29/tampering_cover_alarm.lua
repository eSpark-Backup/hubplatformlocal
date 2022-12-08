local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/tampering_cover_alarm")()
local tampering_cover_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/tampering_cover_events")

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/append_to_table")(
    item.enum,
    tampering_cover_enum.no_tampering_cover,
    tampering_cover_enum.tampering_cover
)

return item
