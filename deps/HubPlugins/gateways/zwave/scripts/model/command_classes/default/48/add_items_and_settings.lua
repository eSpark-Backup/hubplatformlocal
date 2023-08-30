local self, inclusion_context, build_tree_context, device_node, items, settings = ...

local items_without_child_device = hubplugins.weak_require("HUB:zwave/scripts/resources/items_without_child_device")

self:call("add_compicated_items_settings", inclusion_context, build_tree_context, device_node, items, settings, items_without_child_device.sensor_binary)
