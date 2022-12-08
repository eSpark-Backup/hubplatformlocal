local self, inclusion_context, build_tree_context, device_node = ...

print(("[base_command_class.process_inclusion] processing of command class: 0x%X (channel %d)"):format(build_tree_context.command_class.id, build_tree_context.channel.id))

local already_processed_cc = inclusion_context.processed_command_class_ids[build_tree_context.channel.id][build_tree_context.command_class.id]
if already_processed_cc then
    return
end

self:call("process_inclusion_impl", inclusion_context, build_tree_context, device_node)
self:call("mark_processed_cc", inclusion_context, build_tree_context, device_node)
