local self, item_descriptor, value = ...

local zwave = require("zwave")

zwave.set_sound_switch_configuration(item_descriptor.node_id, item_descriptor.channel_id, 0xFF, value)

zwave.request_sound_switch_configuration(item_descriptor.node_id, item_descriptor.channel_id)
