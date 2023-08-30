local self, item, postponed, predicate = ...

local ids = {}

for id, value in pairs(item.value) do
    if predicate == nil or predicate(value) then
        table.insert(ids, id)
    end
end

table.sort(ids, function(a, b)
        return tonumber(a) < tonumber(b)
    end)

self:call(postponed and "add_postponed_requests" or "add_requests", ids)
