local self, inclusion_context, build_tree_context, device_node = ...

local items = {}

for _, sensor in ipairs(build_tree_context.command_class.sensors) do
    if sensor.sensor_type == "general_purpose" then
        local item_names = require("HUB:zwave/scripts/resources/item_names")
        local multilevel_sensor_map = hubplugins.weak_require("HUB:zwave/scripts/resources/multilevel_sensor_map")
        local item_descriptor_tags = require("HUB:zwave/scripts/resources/item_descriptor_tags")
        local item = self:call("create_item", inclusion_context, build_tree_context, device_node, multilevel_sensor_map.sensor_type_to_item_name_method(item_names.motion), item_descriptor_tags.access_control)
        if item then
            items[#items + 1] = item
        end    
     else
        local item = self:call("get_item", inclusion_context, build_tree_context, device_node, sensor.sensor_type)
        if item then
            items[#items + 1] = item
        end     
    end
end

return items
