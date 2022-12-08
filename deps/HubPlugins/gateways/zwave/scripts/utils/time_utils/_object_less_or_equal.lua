local self, lhs, rhs, fields = ...

for _, field in ipairs(fields) do
    if lhs[field] < rhs[field] then
        return true
    elseif lhs[field] > rhs[field] then
        return false
    end
end

return true
