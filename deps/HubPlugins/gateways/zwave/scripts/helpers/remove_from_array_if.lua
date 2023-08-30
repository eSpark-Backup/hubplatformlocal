local array, predicate = ...

local src = 1
local dst = 1
while array[src] ~= nil do
    if not predicate(array[src], src) then
        array[dst] = array[src]
        dst = dst + 1
    end

    src = src + 1
end

for i = src - 1, dst, -1 do
    array[i] = nil
end

return src - dst
