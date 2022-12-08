local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item_name = item_names.switch
local switch_item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
local switch_value = value ~= 0x00 and true or false


self:call("update_value_in_core", item_name, switch_item_id, switch_value)

self:_super_call("default", "update_multilevel_switch", node_id, channel_id, class_id, value, target_value, target_duration)
