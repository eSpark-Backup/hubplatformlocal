local self, item_id = ...

local table_key = "postponed_state_end_time_" .. item_id

if self._storage.exists(table_key) then
    self._storage.delete(table_key)
end
