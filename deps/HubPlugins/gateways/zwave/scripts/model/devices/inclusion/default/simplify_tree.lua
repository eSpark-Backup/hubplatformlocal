local self, inclusion_context = ...

local tree = inclusion_context.device_tree_model

local is_self_sufficient_device = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/is_self_sufficient_device")

tree:call("merge_nodes_to_parents",
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/nodes_merger_with_info"),
    function(device_node, parent_node)
        return not (tree:call("has_siblings", device_node) or is_self_sufficient_device(tree:call("get_data", parent_node)))
    end
)
