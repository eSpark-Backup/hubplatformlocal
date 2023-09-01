local self, device_id, node_id = ...
node_id = math.floor(node_id)

print("LUA: device mapping added " .. device_id .. " < --- > " .. node_id)
self._storage.set_number(device_id, node_id)
self._storage.set_string(node_id, device_id)
