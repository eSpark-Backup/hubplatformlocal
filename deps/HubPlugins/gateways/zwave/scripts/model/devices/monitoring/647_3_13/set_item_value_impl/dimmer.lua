local self, item_descriptor, value, item_id = ...

local zwave = require("zwave")

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local dimmer_item_id = self:call("find_item_id", 
                                item_descriptor.item_name, 
                                item_descriptor.node_id, 
                                item_descriptor.channel_id, 
                                item_descriptor.class_id)

local switch_item_id = self:call("find_item_id", 
                                item_names.switch, 
                                item_descriptor.node_id, 
                                item_descriptor.channel_id, 
                                class_ids.switch_binary)


local helper = hubplugins.weak_require("HUB:zwave/scripts/helpers/multilevel_switch_helper")
value = helper.value_from_ui(value)

zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

self:call("update_value_in_core", item_descriptor.item_name, dimmer_item_id, value)
self:call("update_value_in_core", item_names.switch, switch_item_id, value ~= 0)

self:call("request_value_with_delay", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
