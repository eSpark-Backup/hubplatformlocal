local self, value = ...

local XYZ_value = self:call("xyY_to_XYZ", value)

return self:call("XYZ_to_RGB", XYZ_value)