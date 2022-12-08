local self, item_descriptor, value = ...

local delay = 500
local timer_args = self._timer.get_arguments(self._timer_id)

if timer_args then
    self._timer.cancel(self._timer_id)
    timer_args.item_descriptor = item_descriptor
    timer_args.value = value
    timer_args.counter = timer_args.counter or 0
    timer_args.max_counter = self._max_counter
else
    timer_args = {
        item_descriptor = item_descriptor,
        value = value,
        node_id = self._node_id,
        channel_id = self._channel_id,
        counter = 0,
        max_counter = self._max_counter
    }
end

self._timer.set_interval_with_id(
    delay,
    self._timer_id,
    "HUB:zwave/scripts/timer_handlers/timeout/set_thermostat_setpoint",
    timer_args)
