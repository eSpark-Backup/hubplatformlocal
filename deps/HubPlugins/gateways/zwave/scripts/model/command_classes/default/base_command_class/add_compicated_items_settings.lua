local self, inclusion_context, build_tree_context, device_node, items, settings, items_without_child_device = ...

for _, item in pairs(items) do
    local  current_dev_node = device_node
    if items_without_child_device[item.name] == nil then
        current_dev_node = self:call("add_device_if_needed", inclusion_context, device_node, item.name)
    end

    self:call("add_item", inclusion_context, build_tree_context, current_dev_node, item)
    if settings and settings[item.name] ~= nil then
        for _, setting in ipairs(settings[item.name]) do
            local device_node_data = inclusion_context.device_tree_model:call("get_data", current_dev_node)
            table.insert(device_node_data.settings, setting)
        end
    end

    hubplugins.try_collectgarbage()
end
