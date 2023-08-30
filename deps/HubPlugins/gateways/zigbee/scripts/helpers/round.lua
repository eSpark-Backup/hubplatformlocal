local number, precision = ...

local factor = 10^precision
return math.floor(number * factor + 0.5) / factor
