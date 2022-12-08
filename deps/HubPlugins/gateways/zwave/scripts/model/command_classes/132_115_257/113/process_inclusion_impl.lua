local self, inclusion_context, build_tree_context, device_node = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")

local items = {}

local item_tag_name =  require("HUB:zwave/scripts/resources/item_descriptor_tags").water_alarm
local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.water_leak_alarm, item_tag_name)
items[item.name] = item

item_tag_name = require("HUB:zwave/scripts/resources/item_descriptor_tags").heat_alarm
item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.heat_alarm, item_tag_name)
items[item.name] = item

item_tag_name = require("HUB:zwave/scripts/resources/item_descriptor_tags").power_management
item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.battery_maintenance_state, item_tag_name)
items[item.name] = item

item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.power_state, item_tag_name)
items[item.name] = item

item_tag_name = require("HUB:zwave/scripts/resources/item_descriptor_tags").system
item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.hw_state, item_tag_name)
items[item.name] = item

local items_without_child_device = hubplugins.weak_require("HUB:zwave/scripts/resources/items_without_child_device")

self:call("add_complicated_items", inclusion_context, build_tree_context, device_node, items, items_without_child_device.notifications)
