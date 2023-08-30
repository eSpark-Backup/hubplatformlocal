local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class

if not hubplugins.device_settings_enabled then return end

if cc.version <= 2 then
    self:call("process_old_versions", inclusion_context, build_tree_context, device_node)
else
    self:call("process_new_versions", inclusion_context, build_tree_context, device_node)
end
