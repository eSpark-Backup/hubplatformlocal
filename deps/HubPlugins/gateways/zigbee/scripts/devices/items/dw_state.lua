local item_names = hubplugins.weak_require("HUB:zigbee/scripts/resources/item_names")
local dw_state_enum = hubplugins.weak_loadfile("HUB:zigbee/scripts/resources/enums/dw_state_event")()

return {
    name = item_names.dw_state,
    value_type = "token",
    enum = {
        dw_state_enum.unknown,
        dw_state_enum.dw_is_closed,
        dw_state_enum.dw_is_opened },
    value = "unknown",
    has_getter = true,
    has_setter = false
}
