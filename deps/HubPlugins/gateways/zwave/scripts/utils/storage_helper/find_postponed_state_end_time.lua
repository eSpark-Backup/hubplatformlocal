local self, item_id = ...

local table_key = "postponed_state_end_time_" .. item_id

return self._storage.get_table(table_key)
