local self, device_id, node_id = ...

self._storage.set_string(device_id, node_id)
self._storage.set_string(node_id, device_id)
