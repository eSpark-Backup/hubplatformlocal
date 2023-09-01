local self, node_id, class_id, tag = ...

local key = self:call("create_setting_descriptor_key", node_id, class_id, tag)

return self._storage.get_string(key)
