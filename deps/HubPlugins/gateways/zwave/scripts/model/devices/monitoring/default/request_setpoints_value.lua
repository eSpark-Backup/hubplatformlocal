local self, node_id, channel_id, setpoints, initial_delay = ...

local timer = require("timer")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local timer_args = {
    node_id = node_id,
    channel_id = channel_id,
    type = nil,
}

local delay = initial_delay or 0
for _, setpoint in ipairs(setpoints) do
    local timer_id = storage:call("create_pending_operation_key", node_id, channel_id, "request_setpoint_value", { math.floor(setpoint.type) })

    timer_args.type = setpoint.type

    print("request_setpoint_value " .. setpoint.type)
    timer.set_timeout_with_id(delay, timer_id, "HUB:zwave/scripts/timer_handlers/timeout/request_setpoint_value", timer_args)

    delay = delay + 2000
end
