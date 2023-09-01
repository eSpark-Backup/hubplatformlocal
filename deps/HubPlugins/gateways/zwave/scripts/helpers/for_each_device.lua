local node_id, device_visit_function = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local root_dev_id = storage:call("find_device_id", node_id)

local devices = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_core_devices")(root_dev_id)
for _, dev in ipairs(devices) do
    device_visit_function(dev)
end
