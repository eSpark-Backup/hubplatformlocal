local self, storage_table_name, key = ...

local table = self:call("get_table", storage_table_name)
return table and table[key]
