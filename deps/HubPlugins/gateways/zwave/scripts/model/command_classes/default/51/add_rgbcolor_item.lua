local self, inclusion_context, build_tree_context, device_node = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.rgb_color)

local item_value = item.value
if build_tree_context.command_class.component_ids ~= nil then
    for _, color_id in ipairs(build_tree_context.command_class.component_ids) do
       item_value[color_id] = 0
    end
else
    print("Color components of RGB item are absent")
end

self:call("add_item", inclusion_context, build_tree_context, device_node, item)
