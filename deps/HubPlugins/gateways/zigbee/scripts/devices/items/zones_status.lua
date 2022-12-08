local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local zone_status_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/zone_status")()

local item = {
    name = item_names.zones_status,
    value_type = "token",
    enum = {
        zone_status_enum.disarmed,
        zone_status_enum.stay,
        zone_status_enum.night,
        zone_status_enum.away },
    value = zone_status_enum.disarmed,
    has_getter = true,
    has_setter = true
}

return item
