local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local door_lock_state = hubplugins.weak_require("HUB:zigbee/scripts/resources/enums/door_lock_state")

return { 
    value_type = "token",
    value = door_lock_state.unknown,
    has_getter = true,
    name = item_names.door_lock,
    has_setter = true,
    enum = {
        door_lock_state.unsecured,
        door_lock_state.secured,
        door_lock_state.unknown
    },
}
