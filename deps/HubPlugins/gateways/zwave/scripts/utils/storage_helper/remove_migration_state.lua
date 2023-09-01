local self = ...

if self._storage.exists("migration_state") then
    self._storage.delete("migration_state")
end
