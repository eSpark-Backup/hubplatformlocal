local self, node_id = ...

local ids = self._storage.get_table("restricted_node_ids")

return ids and ids[node_id] and true or false
