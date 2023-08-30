local year, month, day, hour, minute, second = ...

assert(month >= 1 and month <= 12)
assert(day >= 1 and day <= 31)
assert(hour >= 0 and hour < 24)
assert(minute >= 0 and minute < 60)
assert(second >= 0 and second < 60)

return string.format("%04d-%02d-%02dT%02d:%02d:%02d", year, month, day, hour, minute, second)