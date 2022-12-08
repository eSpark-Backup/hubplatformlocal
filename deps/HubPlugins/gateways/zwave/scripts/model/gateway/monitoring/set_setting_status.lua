local self, setting_id, status = ...

local core = require("core")

if core.set_gateway_setting_status then
    core.set_gateway_setting_status(setting_id, status)
end
