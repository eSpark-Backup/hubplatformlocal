local self, item_name, node_id, class_id, channel_id, tag = ...

local key = self:call("create_item_descriptor_key", item_name, node_id, class_id, channel_id, tag)
return self._storage.get_string(key)
