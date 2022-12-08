local self, node_id, channel_id, class_id, value, target_value = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_name = item_names.dimmer
local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_ids.basic)

self:call("update_value_in_core", item_name, item_id, value)
