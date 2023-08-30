local self, setting_id, descriptor = ...

local key = self:call("create_setting_descriptor_key", descriptor.node_id, descriptor.class_id, descriptor.tag)

self._storage.set_string(key, setting_id)
self._storage.set_table(setting_id, descriptor)
