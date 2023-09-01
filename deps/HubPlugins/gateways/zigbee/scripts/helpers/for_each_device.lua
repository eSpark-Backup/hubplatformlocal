local root_dev_id, device_visit_function, core = ...

-- TODO: add field child_devices to device in core and reimplement this method
local devices_map = {}
for _, dev in ipairs(core.get_devices()) do
    dev.children = {}
    devices_map[dev.id] = dev
end

for _, dev in pairs(devices_map) do
    local parent_dev = devices_map[dev.parent_device_id]
    if parent_dev then
        table.insert(parent_dev.children, dev)
    end
end

local function direct_walk(device, device_visit_function)
    device_visit_function(device)

    local idx, child = next(device.children, nil)
    while(idx)
    do
        direct_walk(child, device_visit_function)
        idx, child = next(device.children, idx)
    end
end

direct_walk(devices_map[root_dev_id], device_visit_function)
