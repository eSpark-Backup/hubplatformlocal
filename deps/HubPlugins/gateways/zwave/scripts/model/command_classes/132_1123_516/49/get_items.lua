local self, inclusion_context, build_tree_context, device_node = ...

local items = {}
for _, sensor in ipairs(build_tree_context.command_class.sensors) do
    if sensor.sensor_type == "general_purpose" then
        local item = self:call("get_item", inclusion_context, build_tree_context, device_node, sensor.sensor_type)
        if item then
            items[#items + 1] = item
        end
    end
end

return items
