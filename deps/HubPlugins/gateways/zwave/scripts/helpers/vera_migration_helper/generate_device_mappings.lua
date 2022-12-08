local self, vera_devices_model = ...

local vera_devices = vera_devices_model.devices
local zwave_network_device_id = self:call("_find_zwave_network_device_id", vera_devices)

if not zwave_network_device_id then
    print("Unable to find Vera zwave network device id")
    return
end

for _, vera_device in ipairs(vera_devices) do
    if vera_device.id_parent == zwave_network_device_id then
        self:call("_process_vera_device", vera_device)
    end
end
