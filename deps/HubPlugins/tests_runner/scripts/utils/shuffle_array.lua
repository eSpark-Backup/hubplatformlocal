local array = ...

for i = #array, 2, -1 do
    local new_index = math.random(1, i)
    array[i], array[new_index] = array[new_index], array[i]
end

return array