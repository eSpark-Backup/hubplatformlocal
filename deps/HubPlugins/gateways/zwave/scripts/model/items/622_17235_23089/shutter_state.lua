local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/shutter_state")()
local shutter_states_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_states")

item.enum = {
    shutter_states_enum.idle,
    shutter_states_enum.learn_upper,
    shutter_states_enum.learn_lower,
    shutter_states_enum.learn_favorite
}

return item
