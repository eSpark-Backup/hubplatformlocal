local self, parent_node, node, data_merge_function = ...

self:call("remove_node", node)

local existing_node
if parent_node then
    existing_node = parent_node.children and parent_node.children[ node.id ]
else
    existing_node = self._root
end

if existing_node then
    if data_merge_function then
        data_merge_function(node.data, existing_node.data)
    end
    self:call("for_each_child", node, function (child) self:call("insert_node", existing_node, child, data_merge_function) end )
    node = existing_node

elseif parent_node then
    node.parent = parent_node
    parent_node.children = parent_node.children or {}
    parent_node.children[ node.id ] = node

elseif self._root == nil then
    self._root = node
end

return node
