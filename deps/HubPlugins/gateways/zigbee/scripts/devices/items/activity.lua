local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local activity_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/activity")()

return {
    name = item_names.activity,
    value_type = "token",
    enum = {
        activity_enum.no_activity,
        activity_enum.shake,
        activity_enum.tilt,
        activity_enum.drop },
    value = activity_enum.no_activity,
    has_getter = true,
    has_setter = false
}
