local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local items = {
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.barrier),
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.barrier_state),
}

self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)

local cc = build_tree_context.command_class

if cc.signals then
    for _, signal in ipairs(cc.signals) do    
        if signal == "audible_notification" or signal == "visual_notification" then
            local child_device_identifier = "barrier_" .. signal
            local switch_item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.switch, signal)
            self:call("add_signal_child_device", inclusion_context, build_tree_context, device_node, child_device_identifier, { switch_item })
        end
    end
end
