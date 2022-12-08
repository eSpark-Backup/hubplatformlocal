local self, descriptor = ...

local key = self:call("create_setting_descriptor_key", descriptor)
return self._storage.get_string(key)
