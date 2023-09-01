local self, item, postponed = ...

local ids = {}

for id in pairs(item.value) do
    table.insert(ids, id)
end

table.sort(ids, function(a, b)
        return tonumber(a) < tonumber(b)
    end)

self:call(postponed and "add_postponed_requests" or "add_requests", ids)
