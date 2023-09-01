local self, node_id, channel_id, class_id, item_name, value = ...

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
assert(item_id, "[handler.update_item_value]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))

self:call("update_value_in_core", item_name, item_id, value)
