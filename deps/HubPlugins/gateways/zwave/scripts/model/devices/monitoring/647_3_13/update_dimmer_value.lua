local self, node_id, channel_id, class_id, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local switch_item_id = self:call("find_item_id", item_names.switch, node_id, channel_id, class_ids.switch_binary)
self:call("update_value_in_core", item_names.switch, switch_item_id, value ~= 0)

local dimmer_item_id = self:call("find_item_id", item_names.dimmer, node_id, channel_id, class_id)
self:call("update_value_in_core", item_names.dimmer, dimmer_item_id, value)

