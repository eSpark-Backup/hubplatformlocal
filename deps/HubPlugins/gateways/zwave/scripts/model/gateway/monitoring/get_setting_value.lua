local self, setting_id = ...

local core = require("core")

if core.get_gateway_setting then
    local setting = assert(core.get_gateway_setting(setting_id))
    return setting.value
end
