local item_names = require("HUB:zwave/scripts/resources/item_names")
local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")

return {
    name = item_names.sound_playback,
    value_type = "token",
    enum = {
        sound_playback_enum.unknown,
        sound_playback_enum.stop,
        sound_playback_enum.play
    },
    value = sound_playback_enum.unknown,
    has_getter = true,
    has_setter = true
}
