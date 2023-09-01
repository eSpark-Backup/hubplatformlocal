local self, quantity, from, to = ...

if from == to then
    return quantity
end

local conversion = ("_from_%s_to_%s"):format(from, to)

return self:call(conversion, quantity)
