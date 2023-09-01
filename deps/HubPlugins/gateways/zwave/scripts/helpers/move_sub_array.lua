local array, dst_index, src_index, len = ...

if dst_index == src_index or len == 0 then
    return
end

local temp = {}
for i = 0, len - 1 do
    table.insert(temp, array[src_index + i])
end

if src_index < dst_index then
    for i = src_index, dst_index - 1 do
        array[i] = array[i + len]
    end
else
    for i = src_index - 1, dst_index, -1 do
        array[i + len] = array[i]
    end
end

for i = 0, len - 1 do
    array[dst_index + i] = temp[i + 1]
end
