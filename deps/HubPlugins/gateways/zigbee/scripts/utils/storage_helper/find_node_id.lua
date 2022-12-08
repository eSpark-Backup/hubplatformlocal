local self, device_id = ...

local result = self._storage.get_number(device_id)
if result then
    return math.floor(result)
end
