local self, splitted_values, indexes_map, slot, value = ...

assert(#splitted_values == #indexes_map)

local week_days_order = require("HUB:zwave/scripts/resources/week_days_order")

local value_week_day_index = week_days_order[value.weekDay]
assert(value_week_day_index)

local new_index
local from, to

local i = #splitted_values
while splitted_values[i] and indexes_map[i].unknown do
    local intervals_equal = splitted_values[i].value.startTime == value.startTime and 
                            splitted_values[i].value.stopTime == value.stopTime
    local found_i
    local next_i = self:call("_handle_frame", splitted_values, i, -1, intervals_equal and function( i, val )
        found_i = found_i or value_week_day_index > week_days_order[val.weekDay] and i + 1
    end)
    
    found_i = found_i or intervals_equal and next_i + 1

    if found_i and
       (found_i == i + 1 or week_days_order[splitted_values[found_i].value.weekDay] ~= value_week_day_index ) then
        new_index = found_i
        from, to = next_i + 1, i + 1
        break
    end

    i = next_i
end

new_index = new_index or from or (#splitted_values + 1)
local prev_group = from and splitted_values[from].group

local group = 1
for _, sv in ipairs(splitted_values) do
    group = math.max(group, sv.group + 1)
end

table.insert(splitted_values, new_index, { value = value })
table.insert(indexes_map, new_index, { 
    slot = slot, 
    unknown = true
})

from = from or new_index
to = to or new_index

for i = from, to do
    splitted_values[i].group = group
end

if not self._device_supports_shifted_days and ((value.startTime == "00:00:00") ~= (value.stopTime == "23:59:00")) then
    local adjucent = prev_group and 
        ( splitted_values[to + 1] and splitted_values[to + 1].group == prev_group and splitted_values[to + 1] or
          splitted_values[from - 1] and splitted_values[from - 1].group == prev_group and splitted_values[from - 1])

    self:call("_try_merge_shifted_week_days", 
               splitted_values, 
               indexes_map, 
               from, 
               to,
               group + 1)

    if adjucent then
        for next_i, i in self:call("_unknown_frame_pairs", splitted_values, indexes_map) do
            if splitted_values[i] == adjucent or splitted_values[next_i + 1] == adjucent then
                self:call("_try_merge_shifted_week_days", 
                      splitted_values, 
                      indexes_map, 
                      next_i + 1, 
                      i,
                      group + 2)
                break
            end    
        end
    end
end
