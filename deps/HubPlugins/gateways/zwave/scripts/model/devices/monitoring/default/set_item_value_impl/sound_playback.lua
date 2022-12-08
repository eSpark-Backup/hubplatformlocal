local self, item_descriptor, value = ...

local zwave = require("zwave")

local sound_playback_enum = hubplugins.weak_require("HUB:zwave/scripts/resources/enums/sound_playback")

if value == sound_playback_enum.stop then
    zwave.set_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id, 0, 0)
elseif value == sound_playback_enum.play then
    local item_names = require("HUB:zwave/scripts/resources/item_names")
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")
    local sound_select_item = self:call(
        "find_item", 
        item_names.sound_select, 
        item_descriptor.node_id, 
        item_descriptor.channel_id, 
        class_ids.sound_switch
    )
    
    local tone_id = sound_select_item and sound_select_item.value or 0xFF

    zwave.set_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id, tone_id, 0)
end

zwave.request_sound_switch_tone_play(item_descriptor.node_id, item_descriptor.channel_id)
