local self, vera_device = ...

-- Create structure with required mappings
local device_data = {}

if vera_device.altid and tonumber(vera_device.altid) then
    device_data.device_id = self._storage:call("find_device_id", tonumber(vera_device.altid))
end

-- Add mappings to core
if next(device_data) ~= nil then
    self._core.add_vera_device_mapping(vera_device.id, device_data)
end
