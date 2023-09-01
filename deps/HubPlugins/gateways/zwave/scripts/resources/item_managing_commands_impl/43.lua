local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Scene Activation
local commands = {
    [item_names.button_state] = {"SCENE_ACTIVATION_SET"}
}

return commands[item.descriptor.item_name]
