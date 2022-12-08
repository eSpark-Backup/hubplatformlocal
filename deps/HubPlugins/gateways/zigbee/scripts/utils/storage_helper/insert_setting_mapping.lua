local self, setting_id, setting_descriptor = ...

self._storage.set_string(self:call("create_setting_descriptor_key", setting_descriptor), setting_id)
self._storage.set_table(setting_id, setting_descriptor);
