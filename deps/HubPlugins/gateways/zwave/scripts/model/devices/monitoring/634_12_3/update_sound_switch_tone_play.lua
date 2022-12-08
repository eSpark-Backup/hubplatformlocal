local self, node_id, channel_id, class_id, tone_id, volume = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")

if tone_id ~= 0 then
    self:call("update_item_value", node_id, channel_id, class_id, item_names.sound_playback, sound_playback_enum.play)
else
    local timer_obj = hubplugins.object(
        "poll_value_timer",
        "HUB:zwave/scripts/timer_handlers",
        node_id,
        channel_id,
        class_id,
        "request_sound_switch_tone_play"
    )

    if timer_obj:call("exists") then
        timer_obj:call("stop")
        print("Stopped " .. timer_obj._timer_id)
    end

    self:call("update_item_value", node_id, channel_id, class_id, item_names.sound_playback, sound_playback_enum.stop)
end
