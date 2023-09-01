local string_time = ...

local year, mounth, day, hour, minute, second = string.match(string_time, "(%d+)-(%d+)-(%d+)T(%d+):(%d+):(%d+)")

return {
    year = tonumber(year),
    mounth = tonumber(mounth),
    day = tonumber(day),
    hour = tonumber(hour),
    minute = tonumber(minute),
    second = tonumber(second)
}
