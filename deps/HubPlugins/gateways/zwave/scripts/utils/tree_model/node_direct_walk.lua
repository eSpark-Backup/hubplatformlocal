local self, node, node_visit_function = ...

node_visit_function(assert(node))
self:call("for_each_child", node, function (node) self:call("node_direct_walk", node, node_visit_function) end)
