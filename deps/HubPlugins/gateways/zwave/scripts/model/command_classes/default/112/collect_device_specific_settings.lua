local self, device_node = ...

local settings_count = device_node.data.info.settings_count or 0
local dev_handler_id = device_node.data.info.device_type_id

local result = {}

for idx = 1, settings_count do
    local setting_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_setting_info")(dev_handler_id, idx)
    result[ setting_info.configuration.number ] = idx
end

return result
