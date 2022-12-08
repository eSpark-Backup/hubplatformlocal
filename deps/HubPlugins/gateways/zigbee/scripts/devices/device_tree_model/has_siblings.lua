local self, device_node = ...

local parent = self:call("get_parent", device_node)
return  parent ~= nil
    and next( parent.children, assert( next( parent.children ) ) ) ~= nil
