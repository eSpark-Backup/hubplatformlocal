local self, node_id, device_info = ...

self:_register("default", "HUB:zwave/scripts/model/devices/monitoring")

self._node_id = node_id
self._device_info = device_info
