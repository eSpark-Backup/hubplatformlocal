local self, node_id, channel_id, door_mode, door_condition = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local item_id = self:call("find_item_id", item_names.door_lock, node_id, channel_id, class_ids.door_lock)

if not item_id then
    error(self:call("create_item_id_not_found_error_message", item_names.door_lock, node_id, channel_id, class_ids.door_lock))
end

print("stop request_door_lock_state timer")

local request_door_lock_state_timer = hubplugins.object("request_value_timer", 
                                                        "HUB:zwave/scripts/timer_handlers", 
                                                        node_id, channel_id, 
                                                        class_ids.door_lock, 
                                                        "request_door_lock_state")
request_door_lock_state_timer:call("stop")

self:call("update_value_in_core", item_names.door_lock, item_id, door_mode)

local dw_state_item_id = self:call("find_item_id", item_names.dw_state, node_id, channel_id, class_ids.door_lock)
if dw_state_item_id then
    self:call("update_value_in_core", item_names.door_lock, dw_state_item_id, door_condition.door_state)
end

local latch_state_item_id = self:call("find_item_id", item_names.latch_state, node_id, channel_id, class_ids.door_lock)
if latch_state_item_id then
    self:call("update_value_in_core", item_names.door_lock, latch_state_item_id, door_condition.latch_state)
end

