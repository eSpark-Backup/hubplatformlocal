local self, setting_name = ...

local key = "gateway_setting_" .. setting_name

return self._storage.get_table(key)
