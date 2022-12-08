local self, setting_id = ...

local core = require("core")

if core.get_gateway_setting then
    return core.get_gateway_setting(setting_id)
end
