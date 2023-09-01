local self, cc, device_node_data = ...

assert(cc.supported_users_count, "No supported user codes count")

if not device_node_data.info.first_user_code and not device_node_data.info.end_user_code then
    return cc.supported_users_count
end

local first_user_code = tonumber(device_node_data.info.first_user_code or 1)
local end_user_code = tonumber(device_node_data.info.end_user_code or cc.supported_users_count)

return end_user_code - first_user_code + 1
