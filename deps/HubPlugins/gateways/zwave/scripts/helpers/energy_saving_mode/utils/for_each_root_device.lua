local self, filter, callback = ...

local nodes_ids = self._zwave.get_all_nodes_ids()

for _, node_id in ipairs(nodes_ids) do
    local dev_id = self._storage:call("find_device_id", node_id)
    if dev_id then
        local device = self._core.get_device(dev_id)
        if device and (not filter or filter(device)) then
            callback(device)
        end
    end
end
