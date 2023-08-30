local self, node_id = ...

local ids = self._storage.get_table("restricted_node_ids")

if ids and ids[node_id] then
    ids[node_id] = nil
    self:call("storage/set_table", "restricted_node_ids", ids)
end
