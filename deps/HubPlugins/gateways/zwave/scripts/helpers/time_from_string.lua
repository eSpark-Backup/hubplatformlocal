local time_string = ...

local hour, minute, second = time_string:match("(%d+):(%d+):(%d+)")
hour, minute, second = tonumber(hour), tonumber(minute), tonumber(second)

assert(hour and minute and second, ("Invalid format of time string: %s. It must be hh:mm:ss"):format(time_string) )
return hour, minute, second