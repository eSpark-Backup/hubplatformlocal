local self, node_id, channel_id, class_id, volume, tone_id = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local sounding_modes = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sounding_mode_event")

self:call("update_item_value", node_id, channel_id, class_id, item_names.sound_volume, volume)

if volume ~= 0 then
    self:call("update_item_value", node_id, channel_id, class_id, item_names.sounding_mode, sounding_modes.audible)
else
    self:call("update_item_value", node_id, channel_id, class_id, item_names.sounding_mode, sounding_modes.silent)
end

self:call("update_item_value", node_id, channel_id, class_id, item_names.sound_select, tone_id)
