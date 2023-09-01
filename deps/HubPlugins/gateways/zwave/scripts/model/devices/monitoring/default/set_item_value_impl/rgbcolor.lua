local self, item_descriptor, value = ...

local zwave = require "zwave"

zwave.set_rgb_value( item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value )
zwave.request_value( item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id )
