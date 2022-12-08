local self, node_id, device_info = ...

local device_info = device_info or {}

local data = {
    info = setmetatable(device_info, self._mt_base_device_info),
    items = {},
    settings = {},
    -- merged_items_map
}

return self:_super_call("tree_model", "create_node", node_id, data)
