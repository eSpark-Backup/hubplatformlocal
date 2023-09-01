local self, inclusion_context, build_tree_context, device_node, items = ...

for _, item in pairs(items) do
    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
end
