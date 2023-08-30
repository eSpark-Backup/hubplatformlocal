local self, inclusion_context, build_tree_context, device_node, items, items_without_child_device = ...

for _, item in pairs(items) do
    if items_without_child_device[item.name] ~= nil then
        self:call("add_item", inclusion_context, build_tree_context, device_node, item)
    else
        local current_dev_node = self:call("add_device_if_needed", inclusion_context, device_node, item.name)
        self:call("add_item", inclusion_context, build_tree_context, current_dev_node, item)
    end

    hubplugins.try_collectgarbage()
end
