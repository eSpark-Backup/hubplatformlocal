local self, value = ...

local XYZ_value = self:call("RGB_to_XYZ", value)

return self:call("XYZ_to_xyY", XYZ_value)