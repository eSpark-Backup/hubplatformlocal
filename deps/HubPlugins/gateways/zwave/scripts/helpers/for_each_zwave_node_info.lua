local node_info_callback = ...

local zwave = require("zwave")

local nodes_ids = zwave.get_all_nodes_ids()
if not nodes_ids then
    return
end

for _, node_id in ipairs(nodes_ids) do
    local node_info = zwave.get_node_info(node_id)
    node_info_callback(node_info)

    node_info = nil
    hubplugins.try_collectgarbage()
end
