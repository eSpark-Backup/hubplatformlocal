local self, node, node_visit_function = ...

node_visit_function(assert(node))

local nodes = { }
self:call("for_each_child", node, function (node) 
    table.insert( nodes, node )
end)

table.sort( nodes, function( a, b )
    return tostring(a.id) < tostring(b.id)
end )

for _, node in ipairs( nodes ) do
    self:call("node_direct_stable_walk", node, node_visit_function)
end