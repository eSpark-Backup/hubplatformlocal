local self, inclusion_context, build_tree_context, device_node = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local items_adder = self:call("get_items_adder", "co_alarm")
items_adder(inclusion_context, build_tree_context, device_node, {
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.co_alarm),
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.test_state)
})

items_adder = self:call("get_items_adder", "smoke_alarm")
items_adder(inclusion_context, build_tree_context, device_node, {
    self:call("create_item", inclusion_context, build_tree_context, device_node, item_names.smoke_alarm)
})
