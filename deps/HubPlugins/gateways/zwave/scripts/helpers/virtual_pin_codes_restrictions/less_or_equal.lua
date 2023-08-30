local lhs, rhs = ...

local priority_list = { "year", "mounth", "day", "hour", "minute", "second" }

for _, field in ipairs(priority_list) do
    if lhs[field] < rhs[field] then
        return true
    elseif lhs[field] > rhs[field] then
        return false
    end
end

return true
