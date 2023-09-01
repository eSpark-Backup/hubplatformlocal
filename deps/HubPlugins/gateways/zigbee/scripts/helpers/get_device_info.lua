local dev_handler_id = ...

local base_device_info_location = "HUB:zigbee/scripts/devices/device_info/"
local device_info_path =
    hubplugins.weak_loadfile(base_device_info_location .. "generated/" .. dev_handler_id) or
    hubplugins.weak_loadfile(base_device_info_location .. "default")
assert(device_info_path)

return device_info_path()
