local self, setting_id, value, descriptor = ...

local zwave = require("zwave")

if descriptor.tag == "play_default" then
        zwave.set_sound_switch_tone_play(descriptor.node_id, descriptor.channel_id, 0xFF, 0)
        zwave.request_sound_switch_tone_play(descriptor.node_id, descriptor.channel_id)
end
