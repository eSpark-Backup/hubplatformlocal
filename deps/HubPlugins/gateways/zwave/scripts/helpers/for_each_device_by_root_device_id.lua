local device_id, device_visit_function = ...

local devices = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_core_devices")(device_id)
for _, dev in ipairs(devices) do
    device_visit_function(dev)
end
