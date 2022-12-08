local self, splitted_values, indexes_map, from, to, next_group = ...

local adjacent_to
local adjacent_from

local value = splitted_values[from].value

for next_i, i in self:call("_unknown_frame_pairs", splitted_values, indexes_map) do
    if ( splitted_values[next_i] == nil or splitted_values[next_i].group ~= splitted_values[i].group ) and
       ( splitted_values[i + 1] == nil or splitted_values[i + 1].group ~= splitted_values[i].group ) and
       ( ( value.startTime == "00:00:00" and 
                splitted_values[i].value.stopTime == "23:59:00" and 
                splitted_values[i].value.startTime > value.stopTime and 
                self:call("_are_weekdays_adjucent", splitted_values, next_i + 1, i, from, to ) ) or
         ( value.stopTime == "23:59:00" and
                splitted_values[i].value.startTime == "00:00:00" and 
                splitted_values[i].value.stopTime < value.startTime) and
                self:call("_are_weekdays_adjucent", splitted_values, from, to, next_i + 1, i ) ) then
        adjacent_to, adjacent_from = i, next_i + 1
        break
    end
end

local len = to - from + 1

if adjacent_from and adjacent_to then
    local group = next_group or splitted_values[from].group

    for i = from, to do
        local adjacent_i = i - from + adjacent_from
        splitted_values[i].group = group
        splitted_values[adjacent_i].group = group
    end

    local dst = splitted_values[adjacent_from].value.startTime == "00:00:00" and adjacent_from or adjacent_to + 1
    dst = dst > to and dst - len or dst

    local move_sub_array = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/move_sub_array")

    move_sub_array(splitted_values, dst, from, len)
    move_sub_array(indexes_map, dst, from, len)

    return true
end

