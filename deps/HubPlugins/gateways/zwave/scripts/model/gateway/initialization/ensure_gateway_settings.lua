local self = ...

local mandatory_settings = {
    "mains_devices_polling_interval",
    "reachable_check_attempts_count",
    "reachable_check_policy",
    "wake_up_devices_polling_interval",
    "zwave_reconstruct",
    "zwave_reset",
    
    "home_id",
    "locale",
    "node_id",
    "role",
    "version",
    
    "unset_pin_codes_count_to_stop_requesting"
}

for _, setting_name in ipairs(mandatory_settings) do
    self:call("ensure_gateway_setting", setting_name)
end

local zwave = require("zwave")
local info = zwave.get_info()
if info.port then
    self:call("ensure_gateway_setting", "port")
end
