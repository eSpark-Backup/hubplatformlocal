local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local tampering_cover_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/tampering_cover_events")()

local item = {
    name = item_names.tampering_cover_alarm,
    value_type = "token",
    enum = { tampering_cover_enum.unknown,
        tampering_cover_enum.no_tampering_cover,
        tampering_cover_enum.tampering_cover
    },
    value = tampering_cover_enum.unknown,
    has_getter = true,
    has_setter = false
}

return item
