local self,node_id, channel_id, class_id, value, target_value = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").home_security
local item_id = self:call("find_item_id", item_names.motion, node_id, channel_id, class_ids.sensor_binary, item_tag )

self:call("update_value_in_core", item_names.motion, item_id, value > 0)
