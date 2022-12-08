local self, node, table_name = ...

local device_table = node.data.info

if rawget(device_table, table_name) == nil then
    device_table[table_name] = setmetatable({}, { __index = device_table[table_name] })
end

return device_table