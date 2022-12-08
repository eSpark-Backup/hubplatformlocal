local hour, minute, second = ...

assert(hour >= 0 and hour < 24)
assert(minute >= 0 and minute < 60)
assert(second >= 0 and second < 60)

return string.format("%02d:%02d:%02d", hour, minute, second)