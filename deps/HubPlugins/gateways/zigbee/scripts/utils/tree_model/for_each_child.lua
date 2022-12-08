local self, node, node_visit_function = ...

assert(node)
for _, child in pairs(node.children) do
    node_visit_function(child)
end

