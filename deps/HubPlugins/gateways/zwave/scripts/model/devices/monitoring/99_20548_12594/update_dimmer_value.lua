local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

local is_transition_done = self:_super_call("default", "update_dimmer_value", node_id, channel_id, class_id, value, target_value, target_duration)

if is_transition_done and channel_id > 0  then
    local item_name = require("HUB:zwave/scripts/resources/item_names").dimmer

    local value_1, value_2
    if channel_id == 1 then
        local item_other = self:call("find_item", item_name, node_id, 2, class_id)
        assert(item_other, "[handler.update_item_value]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, 2, class_id))
        value_1, value_2 = value, item_other.value
    else
        local item_other = self:call("find_item", item_name, node_id, 1, class_id)
        assert(item_other, "[handler.update_item_value]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, 1, class_id))
        value_1, value_2 = item_other.value, value
    end
    local root_value = value_1 ~= 0 and value_1 or value_2
    self:call("update_item_value", node_id, 0, class_id, item_name, root_value)
end

return is_transition_done
