local self = ...

assert(self._storage_table_prefix)
assert(self._item_id)

return self._storage_table_prefix .. ".slots_mappings." .. self._item_id