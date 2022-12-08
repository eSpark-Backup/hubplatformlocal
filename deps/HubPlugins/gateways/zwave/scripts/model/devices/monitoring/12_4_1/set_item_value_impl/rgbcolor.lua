local self, item_descriptor, value = ...

local zwave = require "zwave"

local supported_colors = {
    {red = 255, green = 0, blue = 0},
    {red = 0, green = 255, blue = 0},
    {red = 0, green = 0, blue = 255},
    {red = 255, green = 255, blue = 0},
    {red = 255, green = 0, blue = 255},
    {red = 0, green = 255, blue = 255},
    {red = 255, green = 255, blue = 255}
}

if not value.red or not value.green or not value.blue then return end

local color_to_set = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/adjust_color")(value, supported_colors)

zwave.set_rgb_value( item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, color_to_set )
zwave.request_value( item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id )
