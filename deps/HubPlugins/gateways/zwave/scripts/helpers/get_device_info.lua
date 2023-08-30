local dev_handler_id = ...

local base_device_info_location = "HUB:zwave/scripts/model/devices/devices_info/"
local device_info_file = hubplugins.weak_loadfile(base_device_info_location .. "generated/" .. dev_handler_id)
                     or  hubplugins.weak_loadfile(base_device_info_location .. "base_info")
    
return device_info_file()
