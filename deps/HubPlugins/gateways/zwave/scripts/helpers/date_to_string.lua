local year, month, day = ...

assert(month >= 1 and month <= 12)
assert(day >= 1 and day <= 31)

return string.format("%04d-%02d-%02d", year, month, day)