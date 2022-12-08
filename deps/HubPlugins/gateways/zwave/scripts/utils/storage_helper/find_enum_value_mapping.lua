local self, object_id = ...

local key = object_id .. "_mapping"

return self._storage.get_table(key)
