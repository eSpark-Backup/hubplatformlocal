local self, node_id, device_id = ...

local map = self._storage.get_table("checking_devices")

if not map then map = {} end

map[tostring(math.floor(node_id))] = device_id

self._storage.set_table("checking_devices", map)
