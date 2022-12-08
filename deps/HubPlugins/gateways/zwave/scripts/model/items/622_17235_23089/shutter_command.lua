local item = hubplugins.weak_loadfile("HUB:zwave/scripts/model/items/default/shutter_command")()
local shutter_commands_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/shutter_commands")

item.enum = {
    shutter_commands_enum.off,
    shutter_commands_enum.store,
    shutter_commands_enum.learn_upper,
    shutter_commands_enum.learn_lower,
    shutter_commands_enum.learn_favorite
}

return item
