local self, item = ...

local key = self:call("create_item_descriptor_key", item)
return self._storage.get_string(key)
