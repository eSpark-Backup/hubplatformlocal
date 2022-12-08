local self, item_name, node_id, class_id, channel_id, tag = ...

local key = item_name .. '_' .. math.floor(node_id) .. '_' .. math.floor(class_id) .. '_' .. math.floor(channel_id)
if tag then
    return key .. '_' .. tag
end

return key
