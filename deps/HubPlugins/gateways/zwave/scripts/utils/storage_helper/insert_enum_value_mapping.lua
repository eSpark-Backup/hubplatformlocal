local self, object_id, values = ...

if values ~= nil then
    local key = object_id .. "_mapping"

    self._storage.set_table(key, values)
end
