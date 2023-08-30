local self, node = ...

assert(node)
if self._root == node then
    self._root = nil
elseif node and node.parent then
    node.parent.children[ node.id ] = nil
    node.parent = nil
end
