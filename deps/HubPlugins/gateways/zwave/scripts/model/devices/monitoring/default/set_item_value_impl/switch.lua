local self, item_descriptor, value = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local zwave = require("zwave")

if item_descriptor.class_id == class_ids.barrier_operator then
    local value_to_set = value == true and 0xFF or 0
    zwave.set_barrier_signal_value(item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.tag, value_to_set)
    zwave.request_barrier_signal_value(item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.tag)
elseif item_descriptor.class_id == class_ids.basic then
    self:call("set_item_value_impl/basic", item_descriptor, value)
elseif item_descriptor.class_id == class_ids.sound_switch then
    local value_to_set = value == true and "play" or "stop"
    item_descriptor.name = "sound_playback"
    self:call("set_item_value_impl/sound_playback", item_descriptor, value_to_set)
else
    self:call("set_item_value_impl/default", item_descriptor, value)
end
