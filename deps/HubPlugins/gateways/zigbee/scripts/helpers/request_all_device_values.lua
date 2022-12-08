local core = assert(require "core", "Fail to load core functionality")

local gateway_id = core.get_gateway().id
local devices = core.get_devices()

local function is_root_device(device)
    return not device.parent_device_id or device.parent_device_id == ""
end

if not devices then return end

for _, device in ipairs(devices) do
    if device.gateway_id == gateway_id and is_root_device(device) then
       hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/request_device_values")(device.id)
    end
end
