local item = ...
local item_names = require( "HUB:zwave/scripts/resources/item_names")
-- Sound Switch
local commands = {
    [item_names.sound_playback] = {"SOUND_SWITCH_TONE_PLAY_SET"}
}

return commands[item.descriptor.item_name]
