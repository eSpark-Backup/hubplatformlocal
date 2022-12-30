local self, node_id, channel_id, class_id, value, target_value, duration = ...

local timer = require "timer"

local function request_channel_value(channel_id_to_request, delay)
    timer.set_timeout(
        delay,
        "HUB:zwave/scripts/timer_handlers/timeout/request_value",
        {
            node_id = node_id,
            class_id = class_id,
            channel_id = channel_id_to_request
        }
    )
end

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

if class_id ~= class_ids.switch_binary then return end

local item = assert(self:call("find_item", item_names.switch, node_id, channel_id, class_id))
local result_value = value ~= 0
if item.value ~= result_value then
    if channel_id == 0 then
        request_channel_value(1, 200)
        request_channel_value(2, 400)
        request_channel_value(3, 600)
    else
        request_channel_value(0, 200)
    end
end

self:_super_call("default", "update_value", node_id, channel_id, class_id, value, target_value, duration)
