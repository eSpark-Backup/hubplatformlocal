local self, sub_values, index, delta, value_cb = ...

local nxt = index and sub_values[index]

if nxt == nil then
    return nil
end

local week_days_order = require("HUB:zwave/scripts/resources/week_days_order")

local current

repeat
    current = nxt
    if value_cb then
        value_cb(index, current.value)
    end

    index = index + delta
    nxt = sub_values[index]
until nxt == nil or
      nxt.group ~= current.group or
      nxt.value.startTime ~= current.value.startTime or
      nxt.value.stopTime ~= current.value.stopTime or
      week_days_order[nxt.value.weekDay] * delta < week_days_order[current.value.weekDay] * delta

return index