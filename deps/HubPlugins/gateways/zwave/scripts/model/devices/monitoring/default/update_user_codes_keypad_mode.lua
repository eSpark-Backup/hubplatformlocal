local self, node_id, channel_id, class_id, mode = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_id = self:call("find_item_id", item_names.user_codes_keypad_mode, node_id, channel_id, class_id)

self:call("update_value_in_core", item_names.user_codes_keypad_mode, item_id, mode)
