local self, storage_table_name, key = ...

local table = self._storage.get_table(storage_table_name)
if table then
    table[key] = nil

    if next(table) then
        self._storage.set_table(storage_table_name, table)
    else
        self._storage.delete(storage_table_name)
    end
end
