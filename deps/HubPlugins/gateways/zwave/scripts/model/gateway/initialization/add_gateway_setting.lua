local self, setting_name = ...

local setting_info = hubplugins.weak_loadfile("HUB:zwave/scripts/model/gateway/settings/" .. setting_name)

if not setting_info then
    print("Unsupported gateway setting name: " .. setting_name)
    return
end

local core = require("core")
if core.add_gateway_setting then
    core.add_gateway_setting(setting_info())
end
