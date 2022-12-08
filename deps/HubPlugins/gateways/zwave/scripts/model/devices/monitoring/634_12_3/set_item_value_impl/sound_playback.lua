local self, item_descriptor, value = ...

local zwave = require("zwave")
local timer = require("timer")

local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")

local function request_sound_switch_tone_play(delay)
    timer.set_timeout(delay, "HUB:zwave/scripts/timer_handlers/common/request_value", {
        node_id = item_descriptor.node_id,
        channel_id = item_descriptor.channel_id,
        method = "request_sound_switch_tone_play",
    })
end

local function poll_sound_switch_tone_play(delay)
    timer.set_timeout(delay, "HUB:zwave/scripts/timer_handlers/common/initiate_polling", {
        node_id = item_descriptor.node_id,
        channel_id = item_descriptor.channel_id,
        class_id = item_descriptor.class_id,
        method = "request_sound_switch_tone_play",
        interval = 1500 -- Must be more than a second
    })
end

if value == sound_playback_enum.play then
    zwave.set_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id, 0xFF, 0)

    -- Note: the device supports uploading custom sounds, however, the sound switch cc info is not updated
    -- The duration of all default tones is 5 seconds
    local tone_duration = 5000

    -- Note: for some reason the device responds with the previous tone ID for the 1st request, so send a 2nd one shortly after

    -- Send 2 GET requests when the tone starts playing
    zwave.request_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id)
    request_sound_switch_tone_play(2000) -- 2 seconds to account for wake up delay

    -- Start polling until the tone stops playing (see update_sound_switch_tone_play.lua)
    -- Note: don't bother sending 2 requests here since polling will take care of it
    poll_sound_switch_tone_play(2000)
elseif value == sound_playback_enum.stop then
    zwave.set_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id, 0, 0)

    -- Note: the device responds with the correct tone ID after it has been set to 0, so no need for 2 requests
    zwave.request_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id)
end
