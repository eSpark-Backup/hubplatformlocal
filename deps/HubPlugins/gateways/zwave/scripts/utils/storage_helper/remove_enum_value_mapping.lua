local self, object_id = ...

local key = object_id .. "_mapping"

self._storage.delete(key)
