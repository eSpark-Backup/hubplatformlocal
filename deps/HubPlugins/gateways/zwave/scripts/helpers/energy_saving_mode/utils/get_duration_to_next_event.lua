local self, event_time = ...

local current_time = self._time_utils:call("get_local_date_time")

local function get_seconds_from_day_begin(hours, minutes, seconds)
    return (hours * 3600) + (minutes * 60) + seconds
end

local event_timestamp

if current_time.year == event_time.year and
   current_time.mounth == event_time.mounth and
   current_time.day == event_time.day then
    event_timestamp = get_seconds_from_day_begin(event_time.hour, event_time.minute, event_time.second)
else
    event_timestamp = get_seconds_from_day_begin(23, 59, 59) + 2
end

return (event_timestamp - get_seconds_from_day_begin(current_time.hour, current_time.minute, current_time.second)) * 1000
