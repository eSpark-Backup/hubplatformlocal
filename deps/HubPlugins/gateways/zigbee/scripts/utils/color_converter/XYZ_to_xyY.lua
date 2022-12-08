local self, value = ...

local x = value.X / ( value.X + value.Y + value.Z)
local y = value.Y / ( value.X + value.Y + value.Z)

return {
    x = x,
    y = y,
    Y = value.Y
}
