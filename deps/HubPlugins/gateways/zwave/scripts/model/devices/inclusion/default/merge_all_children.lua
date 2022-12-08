local self, inclusion_context = ...

local tree = inclusion_context.device_tree_model

tree:call("merge_nodes_to_parents", hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/nodes_merger"))
