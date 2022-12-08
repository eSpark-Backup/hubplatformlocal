local self, item_descriptor, value = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local dimmer_item_id = self:call("find_item_id", 
                                item_names.dimmer, 
                                item_descriptor.node_id, 
                                item_descriptor.channel_id, 
                                class_ids.dimmer)

local switch_item_id = self:call("find_item_id", 
                                item_names.switch, 
                                item_descriptor.node_id, 
                                item_descriptor.channel_id, 
                                class_ids.switch_binary)

if item_descriptor.class_id == class_ids.dimmer then
    self:call("update_value_in_core", item_names.dimmer, dimmer_item_id, value)
    self:call("update_value_in_core", item_names.switch, switch_item_id, value ~= 0)

    self:call("request_value_with_delay", item_descriptor.node_id, item_descriptor.channel_id, class_ids.dimmer)
elseif item_descriptor.class_id == class_ids.switch_binary then
    self:call("update_value_in_core", item_names.dimmer, dimmer_item_id, value == false and 0 or 50)
    self:call("update_value_in_core", item_names.switch, switch_item_id, value)

    self:call("request_value_with_delay", item_descriptor.node_id, item_descriptor.channel_id, class_ids.dimmer)
end
