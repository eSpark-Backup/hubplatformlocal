local self, storage_table_name, key, value = ...

local table = self:call("get_table", storage_table_name) or {}
table[key] = value
self:call("set_table", storage_table_name, table)
