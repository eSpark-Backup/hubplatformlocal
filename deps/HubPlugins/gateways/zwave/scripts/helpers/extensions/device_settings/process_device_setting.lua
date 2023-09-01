local device_type_id, setting_info = ...

local zwave = assert(require("zwave"), "Fail to load the 'zwave' functionality")

if zwave.add_device_specific_configuration then
    zwave.add_device_specific_configuration(device_type_id, setting_info.configuration)
end
