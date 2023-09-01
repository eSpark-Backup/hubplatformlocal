local self, inclusion_context, build_tree_context, device_node = ...

local items = {}
local settings = {}

if not build_tree_context.command_class.sensor_types then
    print("[default.48.get_items] there is no sensor_types")
    return items
end

for _, type in ipairs(build_tree_context.command_class.sensor_types) do
    local item = self:call("get_item", inclusion_context, build_tree_context, device_node, type)
    if item then
        items[item.name] = item
        if hubplugins.device_settings_enabled then
            settings[item.name] = self:call("get_sensor_settings", inclusion_context, build_tree_context, device_node, item.name)
        end
    end

    hubplugins.try_collectgarbage()
end

return items, settings
