local self = ...

if self._indexes_map == nil or self._slots_changes == nil then
    self:call("_load_slots_mappings")
    assert(self._indexes_map, "slots map is not loaded")
    assert(self._slots_changes, "slots changes map is not loaded")
end

return self._indexes_map, self._slots_changes, self._current_slot