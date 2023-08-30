local self, delay, node_id, number, size, value = ...

local timer = require("timer")

local timer_args = {
    node_id = node_id,
    number = number,
    size = size,
    value = value
}

timer.set_timeout(delay, "HUB:zwave/scripts/timer_handlers/common/reset_configuration_value", timer_args)
delay = delay + 2000

timer.set_timeout(delay, "HUB:zwave/scripts/timer_handlers/common/request_configuration_value", timer_args)
delay = delay + 500

return delay
