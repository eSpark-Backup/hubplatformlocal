local self, node_id, node_data = ...

local node = {
    children = {},
    data = node_data,
    id = node_id
}

return node
