local self, item_descriptor, mode = ...

local zwave = require "zwave"

zwave.set_user_codes_keypad_mode(item_descriptor.node_id, item_descriptor.channel_id, mode)

zwave.request_user_codes_keypad_mode(item_descriptor.node_id, item_descriptor.channel_id)
