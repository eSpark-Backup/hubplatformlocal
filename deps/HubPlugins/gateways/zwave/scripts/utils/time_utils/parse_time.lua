local self, string_time = ...

local hour, minute, second = string.match(string_time, "(%d+):(%d+):(%d+)")

return {
    hour = tonumber(hour),
    minute = tonumber(minute),
    second = tonumber(second)
}
