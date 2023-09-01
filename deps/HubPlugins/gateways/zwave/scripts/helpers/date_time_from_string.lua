local date_time_string = ...

local year, month, day, hour, minute, second = date_time_string:match("(%d+)-(%d+)-(%d+)T(%d+):(%d+):(%d+)")

year, month, day, hour, minute, second = 
    tonumber(year), tonumber(month), tonumber(day), tonumber(hour), tonumber(minute), tonumber(second)

assert(year and month and day and hour and minute and second, ("Invalid format of date time string: %s. It must be YYYY-MM-DDThh:mm:ss"):format(date_time_string) )

return year, month, day, hour, minute, second