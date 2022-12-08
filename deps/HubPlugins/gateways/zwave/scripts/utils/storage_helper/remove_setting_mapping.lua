local self, setting_id = ...

local descriptor = self._storage.get_table(setting_id)

if descriptor then
    local key = self:call("create_setting_descriptor_key", descriptor.node_id, descriptor.class_id, descriptor.tag)

    self._storage.delete(setting_id)
    self._storage.delete(key)
end
