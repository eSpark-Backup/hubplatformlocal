local self, parent, source_node_id, dest_node_id, data_merge_function  = ...

local device_node = parent.children[source_node_id]

if device_node then
    self:call("remove_node", device_node)
    device_node.id = dest_node_id
    self:call("insert_node", parent, device_node, data_merge_function)
end
