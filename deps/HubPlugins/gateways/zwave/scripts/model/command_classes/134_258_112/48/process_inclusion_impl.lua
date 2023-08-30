local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_tag = require("HUB:zwave/scripts/resources/item_descriptor_tags").access_control

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.dw_state, item_tag)

self:call("add_item", inclusion_context, build_tree_context, device_node, item)
