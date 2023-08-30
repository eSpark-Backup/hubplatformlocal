local array, element = ...

for i, value in ipairs(array) do
    if value == element then
        return i, value
    end
end
