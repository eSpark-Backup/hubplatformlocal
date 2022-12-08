local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local items = {
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.shutter_second_part),
}

self:call("add_simple_items", inclusion_context, build_tree_context, device_node, items)

self:_super_call("shutter/38", "process_inclusion_impl", inclusion_context, build_tree_context, device_node)
