local self, inclusion_context, build_tree_context, device_node = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")

for _, item_name in ipairs({ item_names.rgb_color_up, item_names.rgb_color_down, item_names.rgb_color_stop }) do
    local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_name)

    if build_tree_context.command_class.component_ids ~= nil then
        for _, color_id in ipairs(build_tree_context.command_class.component_ids) do
            table.insert(item.enum, color_id)
        end
    end

    self:call("add_item", inclusion_context, build_tree_context, device_node, item)
end
