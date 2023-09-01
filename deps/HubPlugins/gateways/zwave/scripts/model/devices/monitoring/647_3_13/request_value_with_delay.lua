local self, node_id, channel_id, class_id, delay = ...

local timer_args = {
    node_id = node_id,
    channel_id = channel_id,
    class_id = class_id,
}

local timer = require("timer")
local timer_id = "iblinds_" .. node_id .. "_" .. math.floor(class_id)

timer.cancel(timer_id)

local default_delay = 120000 -- delay of possible calibration after value set
timer.set_timeout_with_id(delay or default_delay, timer_id, "HUB:zwave/scripts/timer_handlers/common/request_value", timer_args)
