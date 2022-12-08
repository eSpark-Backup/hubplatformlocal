local self, inclusion_context, build_tree_context, device_node = ...

local items = self:call("get_items", inclusion_context, build_tree_context, device_node)
self:call("add_items", inclusion_context, build_tree_context, device_node, items)
