local self, node_id = ...

local ids = self._storage.get_table("restricted_node_ids") or {}

ids[node_id] = true

self:call("storage/set_table", "restricted_node_ids", ids)
