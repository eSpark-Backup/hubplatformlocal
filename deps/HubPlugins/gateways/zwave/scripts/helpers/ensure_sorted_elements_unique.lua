local array, equal_comparator = ...

equal_comparator = equal_comparator or function(a, b) return a == b end

local new_index = 2
local prev_index = 2
while array[prev_index] do
    if not equal_comparator(array[prev_index - 1], array[prev_index]) then
        array[new_index] = array[prev_index]
        new_index = new_index + 1
    end
    prev_index = prev_index + 1
end

for i = prev_index - 1, new_index, -1 do
    array[i] = nil
end

return array
