local self, setting_id, value, descriptor = ...

local zwave = require("zwave")
local core = require("core")

if descriptor.tag == "rediscover" then
    core.update_setting_status(setting_id, "pending")

    zwave.start_rediscover(descriptor.node_id, "HUB:zwave/configs/device_setting.json")
else
    error("Default handler not implemented for setting tag " .. tostring(descriptor.tag))
end


