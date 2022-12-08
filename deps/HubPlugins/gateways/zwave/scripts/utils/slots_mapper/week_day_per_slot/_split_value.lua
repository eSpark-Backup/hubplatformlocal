local self, value, sub_value_handler = ...

if not self._device_supports_shifted_days and value.startTime > value.stopTime then
    local week_days_list = require("HUB:zwave/scripts/resources/week_days_list")
    local week_days_order = require("HUB:zwave/scripts/resources/week_days_order")

    for _, week_day in ipairs(value.weekDays) do
        sub_value_handler({
                weekDay = week_day,
                startTime = value.startTime,
                stopTime = "23:59:00"
            })
    end

    local last = #value.weekDays
    while value.weekDays[last] == week_days_list[7] do
        sub_value_handler({
            weekDay = week_days_list[1],
            startTime = "00:00:00",
            stopTime = value.stopTime
        })
        last = last - 1
    end

    for i = 1, last do
        sub_value_handler({
                weekDay = week_days_list[ week_days_order[ value.weekDays[i] ] % 7 + 1 ],
                startTime = "00:00:00",
                stopTime = value.stopTime
            })
    end
else
    for _, week_day in ipairs(value.weekDays) do
        sub_value_handler({
                weekDay = week_day,
                startTime = value.startTime,
                stopTime = value.stopTime
            })
    end
end

