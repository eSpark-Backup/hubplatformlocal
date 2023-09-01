local self, node, node_visit_function = ...

self:call("for_each_child", node, function (node) self:call("node_reverse_walk", node, node_visit_function) end)
node_visit_function(node)
