local self, item_id = ...

local descriptors = self:call("find_item_descriptors", item_id)
return descriptors and descriptors[1]
