local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local emergency_state = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/emergency_state")()

local item = {
    name = item_names.emergency_state,
    value_type = "token",
    enum = {
        emergency_state.idle },
    value = emergency_state.idle,
    has_getter = true,
    has_setter = false
}

return item
