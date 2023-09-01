local self, week_days, evening_from, evening_to, morning_from, morning_to = ...

if evening_to - evening_from ~= morning_to - morning_from then
    return false
end

local order = require("HUB:zwave/scripts/resources/week_days_order")

while morning_to >= morning_from and
      evening_to >= evening_from and
      ( order[week_days[ morning_from ].value.weekDay] == 1 or 
        order[week_days[ evening_to ].value.weekDay] == 7 ) do
    if order[week_days[ evening_to ].value.weekDay] % 7 + 1 ~=
       order[week_days[ morning_from ].value.weekDay] then
        return false
    end
    morning_from = morning_from + 1
    evening_to = evening_to - 1
end

while morning_to >= morning_from and
      evening_to >= evening_from do
    if order[week_days[ evening_from ].value.weekDay] % 7 + 1 ~=
       order[week_days[ morning_from ].value.weekDay] then
        return false
    end
    morning_from = morning_from + 1
    evening_from = evening_from + 1
end

return true

