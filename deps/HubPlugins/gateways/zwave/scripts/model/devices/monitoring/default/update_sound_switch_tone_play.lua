local self, node_id, channel_id, class_id, tone_id, volume = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")

if tone_id ~= 0 then
    self:call("update_item_value", node_id, channel_id, class_id, item_names.sound_playback, sound_playback_enum.play)
    self:call("update_item_value", node_id, channel_id, class_id, item_names.switch, true)
else
    self:call("update_item_value", node_id, channel_id, class_id, item_names.sound_playback, sound_playback_enum.stop)
    self:call("update_item_value", node_id, channel_id, class_id, item_names.switch, false)
end
