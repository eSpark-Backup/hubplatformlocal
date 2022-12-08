local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local timer_obj = hubplugins.object(
    "poll_value_timer",
    "HUB:zwave/scripts/timer_handlers",
    params.node_id,
    params.channel_id,
    params.class_id,
    params.method
)

timer_obj:call("start", params.interval)

print("Started " .. timer_obj._timer_id)
