--- 'device_node' detaches from a tree in progress of merging
--- @param device_node table
--- @param data_merge_function fun(device_node_data: any, parent_node_data: any)
---
local self, device_node, data_merge_function = ...

local parent_node = self:call("get_parent", device_node)

if not parent_node then
    return
end

local parent_parent_node = self:call("get_parent", parent_node)

self:call("remove_node", device_node)

device_node.id = parent_node.id

self:call("insert_node", parent_parent_node, device_node, data_merge_function)
