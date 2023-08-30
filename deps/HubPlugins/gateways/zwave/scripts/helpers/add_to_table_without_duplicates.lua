local array, values = ...
local set = {}
for _, v in pairs(array) do
    set[v] = true
end

for _, v in pairs(values) do
    if not set[v] then
        table.insert(array, v)
        set[v] = true
    end
end

return array
