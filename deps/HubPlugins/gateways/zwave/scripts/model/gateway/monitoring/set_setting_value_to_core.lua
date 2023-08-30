local self, setting_id, value = ...

local core = require("core")

if core.set_gateway_setting_value then
    core.set_gateway_setting_value(setting_id, value)
end
