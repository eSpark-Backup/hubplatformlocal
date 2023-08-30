local self, inclusion_context, build_tree_context, device_node, items = ...

local items_without_child_device = hubplugins.weak_require("HUB:zwave/scripts/resources/items_without_child_device")

self:call("add_complicated_items", inclusion_context, build_tree_context, device_node, items, items_without_child_device.multilevel_sensors)
