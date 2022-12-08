local self, vera_devices = ...

for _, device in ipairs(vera_devices) do
    if device.device_type == "urn:schemas-micasaverde-com:device:ZWaveNetwork:1" and
       device.invisible == "1" and
       device.category_num == 19 then
        return device.id
    end
end
