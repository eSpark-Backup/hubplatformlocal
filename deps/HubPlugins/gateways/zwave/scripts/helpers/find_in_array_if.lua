local array, predicate = ...

if array then
    for i, value in ipairs(array) do
        if predicate(value) then
            return i, value
        end
    end
end
