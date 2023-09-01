local self, node_id, parameter_number = ...

local timer_args = {
    node_id = node_id,
    number = parameter_number
}

local timer = require("timer")
timer.set_timeout(2000, "HUB:zwave/scripts/timer_handlers/common/request_configuration_value", timer_args)
