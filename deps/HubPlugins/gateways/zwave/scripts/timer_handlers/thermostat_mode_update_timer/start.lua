local self = ...

local delay = 15000 -- the update value handler cancels timer usually and set setpoit directly, if upadate is fallen to set after 15 sec timeout
local timer_args = self._timer.get_arguments(self._timer_id)

if timer_args then
    self._timer.cancel(self._timer_id)
else
    timer_args = {
        node_id = self._node_id,
        channel_id = self._channel_id,
    }
end

self._timer.set_timeout_with_id(
    delay,
    self._timer_id,
    "HUB:zwave/scripts/timer_handlers/timeout/set_thermostat_mode_update",
    timer_args)
