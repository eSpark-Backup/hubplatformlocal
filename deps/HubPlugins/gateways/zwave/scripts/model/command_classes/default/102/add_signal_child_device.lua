local self, inclusion_context, build_tree_context, device_node, child_device_identifier, items = ...

local current_dev_node = self:call("add_device_if_needed", inclusion_context, device_node, child_device_identifier)

self:call("add_simple_items", inclusion_context, build_tree_context, current_dev_node, items)
