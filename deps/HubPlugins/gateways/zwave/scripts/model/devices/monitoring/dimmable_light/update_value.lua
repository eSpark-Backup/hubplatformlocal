local self, node_id, channel_id, class_id, value, target_value, duration = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if class_id == class_ids.switch_binary then
    local item_names = require("HUB:zwave/scripts/resources/item_names")

    local item_name = item_names.switch
    local switch_item_id = self:call("find_item_id", item_name, node_id, channel_id, class_ids.dimmer)

    self:call("update_value_in_core", item_name, switch_item_id, value)

    local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_ids.dimmer)
    request_value_timer:call("start")
else
    self:_super_call("default", "update_value", node_id, channel_id, class_id, value, target_value, duration)
end
