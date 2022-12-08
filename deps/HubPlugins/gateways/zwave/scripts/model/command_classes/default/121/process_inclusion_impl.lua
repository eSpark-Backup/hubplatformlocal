local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local sounding_modes = require("HUB:zwave/scripts/resources/enums/sounding_mode_event")
local sound_playback_enum = require("HUB:zwave/scripts/resources/enums/sound_playback")

local cc = build_tree_context.command_class
local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

local sounding_mode  = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.sounding_mode)
local sound_list     = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.sound_list)
local sound_volume   = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.sound_volume)
local sound_select   = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.sound_select)
local sound_playback = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.sound_playback)
local switch = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.switch)

table.insert(sounding_mode.enum, sounding_modes.audible)
table.insert(sounding_mode.enum, sounding_modes.silent)

for _, tone_info in pairs(cc.tones) do
    sound_list.value[tostring(tone_info.tone_id)] = {
        name = tone_info.tone_name,
        duration = tone_info.duration
    }
end

sound_select.value_max = #cc.tones

local items = {
    sounding_mode,
    sound_list,
    sound_volume,
    sound_select,
    sound_playback, 
    switch
}

self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)

local play_default_setting = self:call("settings/play_default")
play_default_setting.descriptor = {
    node_id = inclusion_context.zwave_node.id,
    class_id = class_ids.sound_switch,
    channel_id = build_tree_context.channel.id,
    tag = "play_default"
}

table.insert(device_node_data.settings, play_default_setting)
