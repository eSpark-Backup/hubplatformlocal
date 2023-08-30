local self, node_id = ...

local map = self._storage.get_table("checking_devices")

if map then
    return map[tostring(math.floor(node_id))]
end
