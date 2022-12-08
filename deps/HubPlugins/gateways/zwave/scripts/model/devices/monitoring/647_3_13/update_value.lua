local self, node_id, channel_id, class_id, value, target_value, duration = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

if class_id == class_ids.switch_binary then
    local dimmer_item_id = self:call("find_item_id", item_names.dimmer, node_id, channel_id, class_ids.dimmer)
    self:call("update_value_in_core", item_names.dimmer, dimmer_item_id, (not value or value == 0) and 0 or 50)
end
    
self:_super_call("default", "update_value", node_id, channel_id, class_id, value, target_value, duration)
