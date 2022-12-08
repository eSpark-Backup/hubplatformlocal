local self, value = ...

local X = (value.Y / value.y) * value.x
local Z = (value.Y / value.y) * (1 - value.x - value.y)

return {
    X = X,
    Y = value.Y,
    Z = Z
}
