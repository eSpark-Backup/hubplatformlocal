local self, base_device_info = ...

self:_init_base("tree_model", "HUB:zwave/scripts/utils")

self:_register("device_tree_model", "HUB:zwave/scripts/model/devices")

self._base_device_info = base_device_info
self._mt_base_device_info = { __index = self._base_device_info }
