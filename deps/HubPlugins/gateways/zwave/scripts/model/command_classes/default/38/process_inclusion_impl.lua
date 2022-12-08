local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local items = {
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.dimmer),
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.dimmer_up),
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.dimmer_down),
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.dimmer_stop)
}

local function auto_detect_window_covering()
    local zwave_node = inclusion_context.zwave_node
    return hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/icon_based/ensure_window_covering")(zwave_node.icon_type)
           or hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/device_info_detection/device_class_based/ensure_window_covering")(zwave_node.generic_type, zwave_node.specific_type, {})
end


local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

if device_node_data.info.category == "window_cov" and device_node_data.info.subcategory == "window_cov" or 
   device_node_data.info.category == "unknown" and auto_detect_window_covering() then
    local goto_favorite = self:call("create_item", 
                                    inclusion_context, 
                                    build_tree_context, 
                                    device_node, 
                                    item_names.goto_favorite, 
                                    item_names.dimmer)
    table.insert(items, goto_favorite)
end


self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)
