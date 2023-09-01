local device_info = ...

-- 1. Try to find specific value for device
if device_info.unset_pin_codes_count_to_stop_requesting then
    return device_info.unset_pin_codes_count_to_stop_requesting
end

-- 2. Try to find gateway setting value
local core = require("core")

if core.get_gateway_setting then
    local setting = core.get_gateway_setting("zwave.unset_pin_codes_count_to_stop_requesting")
    if setting and setting.value ~= nil then
        return setting.value
    end
end

-- 3. Return default value
return 5
