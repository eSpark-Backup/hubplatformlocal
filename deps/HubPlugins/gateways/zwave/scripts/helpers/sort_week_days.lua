local week_days = ...

local order = require("HUB:zwave/scripts/resources/week_days_order")

for _, day in ipairs(week_days) do
    assert(order[day])
end

table.sort(week_days, function(a, b)
    return order[a] < order[b]
end)

return week_days
