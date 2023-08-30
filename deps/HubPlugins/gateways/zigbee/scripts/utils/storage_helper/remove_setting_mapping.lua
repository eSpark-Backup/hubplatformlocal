local self, setting_id = ...

local descriptor = self:call("find_setting_descriptor", setting_id)
if descriptor then
    self._storage.delete(self:call("create_setting_descriptor_key", descriptor))
end

self._storage.delete(setting_id)
