local self, node, node_visit_function = ...

assert(node)
if node.children then
    for _, child in pairs(node.children) do
        node_visit_function(child)
    end
end

