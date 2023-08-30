local self, item_name, node_id, channel_id, class_id, tag = ...

local core = require("core")

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id, tag)
assert(item_id, "[handler.find_item]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))

return core.get_item(item_id)
