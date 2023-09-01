local model, device_type = ...

local device_info_getter =
    (model       and loadfile("HUB:2gig/scripts/model/devices/generated/" .. model:upper())) or
    (device_type and loadfile("HUB:2gig/scripts/model/devices/" .. device_type))

if device_info_getter then
    return device_info_getter()
end
