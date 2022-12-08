local self, inclusion_context, channel = ...

local node_data = self.device_info.endpoints_info and self.device_info.endpoints_info[channel.id]

if node_data and node_data.ignored and node_data.ignored ~= 0 then
    return nil
end


local device_tree_model = inclusion_context.device_tree_model

local root = device_tree_model:call("get_root_node")

local device_node = device_tree_model:call("insert_child", root, channel.id, node_data)
device_node.data.channel_id = channel.id

if node_data and node_data.merge_with then
    inclusion_context.nodes_to_merge[#inclusion_context.nodes_to_merge + 1] = device_node
end

return device_node
