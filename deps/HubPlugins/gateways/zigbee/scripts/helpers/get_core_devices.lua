local dev_id = ...

local core = require("core")

local devices_map = {}
local core_devices = core.get_devices()
if core_devices then
    for _, dev in ipairs(core.get_devices()) do
        local node = {
            dev = dev,
            children = {}
        }
        devices_map[dev.id] = node
    end
end

if not devices_map[dev_id] then
    return {}
end

for _, node in pairs(devices_map) do
    local parent_dev_node = devices_map[node.dev.parent_device_id]
    if parent_dev_node then
        table.insert(parent_dev_node.children, node)
    end
end

local function direct_walk(node, node_visit_function)
    node_visit_function(node)

    local idx, child = next(node.children, nil)
    while(idx)
    do
        direct_walk(child, node_visit_function)
        idx, child = next(node.children, idx)
    end
end

local result = {}

local callback = function(node)
    table.insert(result, node.dev)
end

direct_walk(devices_map[dev_id], callback)

return result
