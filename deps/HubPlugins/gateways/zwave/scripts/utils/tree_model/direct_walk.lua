local self, node_visit_function = ...

if self._root then
    self:call("node_direct_walk", self._root, node_visit_function)
end
