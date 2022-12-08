local self, offset, from, to = ...

if from == to or offset == nil then
    return offset
end

return self:call("convert", offset, from, to) - self:call("convert", 0, from, to)
