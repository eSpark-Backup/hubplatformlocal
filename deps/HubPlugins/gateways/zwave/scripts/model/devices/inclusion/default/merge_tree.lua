local self, inclusion_context = ...

local tree_model = inclusion_context.device_tree_model

local root_node = tree_model:call("get_root_node")
local root_data = tree_model:call("get_data", root_node)
local merge_policy = root_data.info.subdevices_merge_policy or "default"


local need_full_merge = merge_policy == "merge_all" or (merge_policy == "default" and (self:call("is_processed_thermostat", inclusion_context) or self:call("is_processed_doorlock", inclusion_context)))

if need_full_merge then
    self:call("merge_all_children", inclusion_context)
    return
end

local root_node = tree_model:call("get_root_node")

for _, source_node in pairs(inclusion_context.nodes_to_merge) do
    local destination_node = tree_model:call("get_child", root_node, source_node.data.info.merge_with)
    self:call("merge_nodes", inclusion_context, source_node, destination_node)
end

self:call("simplify_tree", inclusion_context)

local new_root = self:call("find_actual_root_device", inclusion_context)
local need_merge_root = 
    root_node == tree_model:call("get_parent", new_root) and merge_policy ~= "separate_root" and 
    (merge_policy == "no_separate_root" 
        or (merge_policy == "default" and not hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_self_sufficient_device")(root_data)))


if need_merge_root then
    tree_model:call("merge_node_to_parent", new_root, hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/nodes_merger_with_info"))
end
