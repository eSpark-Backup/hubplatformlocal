local self, inclusion_context = ...

local tree_model = inclusion_context.device_tree_model

local root_node = tree_model:call("get_root_node")

local nodes_to_merge = {}
inclusion_context.device_tree_model:call("reverse_walk",
    function(node)
        local parent = tree_model:call("get_parent", node)
        if parent and (predicate == nil or predicate(node, parent)) then
            table.insert(nodes_to_merge, node)
        end
    end
)

for _, source_node in pairs(nodes_to_merge) do
    if  source_node.data.info.name ~= "Red" and
        source_node.data.info.name ~= "Green" and
        source_node.data.info.name ~= "Blue" and
        source_node.data.info.name ~= "White" then

        local destination_node = tree_model:call("get_child", root_node, source_node.data.info.merge_with)
        if destination_node then
            self:call("merge_nodes", inclusion_context, source_node, destination_node)
        end
    end
end

self:call("simplify_tree", inclusion_context)

local new_root = self:call("find_actual_root_device", inclusion_context)
local need_merge_root =
        tree_model:call("get_parent", new_root) == root_node
    and (not hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_self_sufficient_device")(tree_model:call("get_data", tree_model:call("get_parent", new_root))))

if need_merge_root then
    tree_model:call("merge_node_to_parent", new_root, hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/nodes_merger_with_info"))
end
