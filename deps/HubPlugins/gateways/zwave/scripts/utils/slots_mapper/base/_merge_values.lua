local self, sub_values = ...

local result = { }
local index = 1

while sub_values[index] do
    local value, next_index = self:call("_merge_value", sub_values, index)
    assert(next_index > index)
    assert(value)

    table.insert(result, value)
    index = next_index
end

return result