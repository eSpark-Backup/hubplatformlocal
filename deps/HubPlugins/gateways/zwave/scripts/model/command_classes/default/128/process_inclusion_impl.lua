local self, inclusion_context, build_tree_context, device_node = ...

local item_name = require("HUB:zwave/scripts/resources/item_names").battery
hubplugins.try_collectgarbage()

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_name)

hubplugins.try_collectgarbage()

self:call("add_item", inclusion_context, build_tree_context, device_node, item)
