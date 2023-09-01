local parent_device_id, main_item_name, index = ...

local core = assert(require "core", "Fail to load core functionality")

local device = loadfile("HUB:water_valve/scripts/devices/" .. main_item_name)()

local gateway = core.get_gateway()
device.gateway_id = gateway.id
device.parent_device_id = parent_device_id

if index ~= nil and index ~= 0 then
    device.name = device.name .. " " .. (index + 1)
end

local dev_id = core.add_device( device )
assert(dev_id, "water_valve: Failed to create water_valve device")

return dev_id
