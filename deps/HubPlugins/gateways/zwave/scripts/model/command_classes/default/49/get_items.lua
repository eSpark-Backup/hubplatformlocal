local self, inclusion_context, build_tree_context, device_node = ...

local items = {}

for _, sensor in ipairs(build_tree_context.command_class.sensors) do
    if sensor.sensor_type == "general_purpose" then
        print("multilevel sensor type is general_purpose. How to handle it?")
    else
        local item = self:call("get_item", inclusion_context, build_tree_context, device_node, sensor.sensor_type)
        items[#items + 1] = item    end
end

return items
