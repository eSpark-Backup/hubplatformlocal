local self, inclusion_context, source_node, destination_node = ...

local tree_model = inclusion_context.device_tree_model

tree_model:call("remove_node", source_node)

source_node.id = destination_node.id

tree_model:call("insert_node", destination_node.parent, source_node, hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/nodes_merger"))
