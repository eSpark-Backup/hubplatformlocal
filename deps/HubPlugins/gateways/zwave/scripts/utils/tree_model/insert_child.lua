local self, parent_node, id, node_data, data_merge_function = ...

local new_node = self:call("create_node", id, node_data)

return self:call("insert_node", parent_node, new_node, data_merge_function)
