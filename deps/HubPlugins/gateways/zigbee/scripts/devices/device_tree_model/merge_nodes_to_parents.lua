--- @param data_merge_function fun(device_node_data: any, parent_node_data: any)
--- @param predicate fun(device_node: any, parent_node: any)
---
local self, data_merge_function, predicate = ...

local nodes_to_merge = {}
self:call("reverse_walk",
    function(node)
        local parent = self:call("get_parent", node)
        if parent and (predicate == nil or predicate(node, parent)) then
            table.insert(nodes_to_merge, node)
        end
    end
)

for _, node in ipairs(nodes_to_merge) do
    self:call("merge_node_to_parent", node, data_merge_function)
end
