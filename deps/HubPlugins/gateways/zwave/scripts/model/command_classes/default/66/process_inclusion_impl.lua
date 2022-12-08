local self, inclusion_context, build_tree_context, device_node = ...

local command_class = build_tree_context.command_class
local item_names = require("HUB:zwave/scripts/resources/item_names")

local item = self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.thermostat_operating_state)

self:call("add_item", inclusion_context, build_tree_context, device_node, item)
