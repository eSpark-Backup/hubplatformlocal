local self, node_id = ...
node_id = math.floor(node_id)

local dev_id = self:call("find_device_id", node_id)
if not dev_id then
    print("[storage_helper.remove_device_mapping] node_id = " .. node_id .. " not found!")
    return
end

self._storage.delete(dev_id)
self._storage.delete(node_id)
