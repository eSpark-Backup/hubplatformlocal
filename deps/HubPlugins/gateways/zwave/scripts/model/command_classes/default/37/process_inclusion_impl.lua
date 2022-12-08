local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.switch)
self:call("add_item", inclusion_context, build_tree_context, device_node, item)
