local self, node_id = ...

local map = self._storage.get_table("checking_devices")

if not map then return end

map[tostring(math.floor(node_id))] = nil

self._storage.set_table("checking_devices", map)
