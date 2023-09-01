local self, inclusion_context, build_tree_context, device_node = ...

local items, settings = self:call("get_items_and_settings", inclusion_context, build_tree_context, device_node)
self:call("add_items_and_settings", inclusion_context, build_tree_context, device_node, items, settings)
