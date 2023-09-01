local self, sub_values, index = ...

local result = { sub_values[index].value }
local group = sub_values[index].group

index = index + 1

if group then
    while sub_values[index] and sub_values[index].group == group do
        table.insert(result, sub_values[index].value)
        index = index + 1
    end
end

return result, index