local self, sub_values, index = ...

local start_index = index

local result = { 
    weekDays = { },
    startTime = sub_values[index].value.startTime,
    stopTime = sub_values[index].value.stopTime
}

index = self:call("_handle_frame", sub_values, index, 1, function(_, value)
    table.insert(result.weekDays, value.weekDay)
end)

local adjacent_value = sub_values[index] and sub_values[index].value
if not self._device_supports_shifted_days and 
    result.stopTime == "23:59:00" and 
    adjacent_value and 
    adjacent_value.startTime == "00:00:00" and 
    adjacent_value.stopTime < result.startTime and
    sub_values[index].group == sub_values[index - 1].group then

    local adjacent_index = self:call("_handle_frame", sub_values, index, 1)

    if self:call("_are_weekdays_adjucent", sub_values, start_index, index - 1, index, adjacent_index - 1) then
        result.stopTime = adjacent_value.stopTime
        index = adjacent_index
    end
end


return result, index
