local param = ...

if type(param) ~= "table" then
    return false
end

local size = #param
local i = 0
for index in pairs(param) do
    i = i + 1
    if type(index) ~= "number" or i > size or index < 1 or index > size then
        return false
    end
end

return size > 0 and i == size
