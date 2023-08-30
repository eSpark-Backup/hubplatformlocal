local self, node_id, channel_id, class_id, value, target_value, target_duration = ...

local item_name = require("HUB:zwave/scripts/resources/item_names").dimmer
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)

if not item_id then -- support basic CC
    class_id = class_ids.basic
    item_id = self:call("find_item_id", item_name, node_id, channel_id, class_id)
end

assert(item_id, "[handler.update_item_value]: " .. self:call("create_item_id_not_found_error_message", item_name, node_id, channel_id, class_id))

local function get_item_value(item_id)
    local core = require("core")

    local item = assert(core.get_item(item_id), "[handler.update_multilevel_switch] item not found with id: " .. item_id)

    return item.value
end

local transition_done

if target_value ~= nil then
    self:call("update_value_in_core", item_name, item_id, value)

    transition_done = value == target_value
else
    if get_item_value(item_id) == value then
        transition_done = true
    else
        self:call("update_value_in_core", item_name, item_id, value)

        transition_done = value == 0 or value == 100
    end
end

local estimated_value_timer = hubplugins.object("estimated_value_timer", "HUB:zwave/scripts/timer_handlers", item_id)
estimated_value_timer:call("stop")

if not transition_done then
--[[
Specification says:
    - PollTime SHOULD be 10 seconds + CommandTime or more
    - PollTime MUST NOT be less than 1 second + CommandTime

We ensure interval 10sec if device reports target duration and 2sec if not.
--]]
    local request_dimmer_value_long_interval = 10000
    local request_dimmer_value_short_interval = 2000
    local estimate_dimmer_value_interval = 1000

    local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, class_id)

    if target_value and target_duration and target_duration > 0 then
        print("Transition in progress - start estimated value timer...")
        estimated_value_timer:call("start", value, target_value, target_duration, estimate_dimmer_value_interval)
        
        request_value_timer:call("start", request_dimmer_value_long_interval)
    else
        print("Transition in progress - requesting current value...")

        request_value_timer:call("start", request_dimmer_value_short_interval)
    end
else
    print("Dimmer transition done")
end

return transition_done
