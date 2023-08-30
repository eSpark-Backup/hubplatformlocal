local self, setting_tag, value = ...

local delay = 3000

local timer_args = self._timer.get_arguments(self._timer_id)

if timer_args then
    self._timer.cancel(self._timer_id)
    timer_args[setting_tag] = value
else 
    timer_args = {
        [setting_tag] = value,
        node_id = self._node_id,
        channel_id = self._channel_id
    }
end

self._timer.set_timeout_with_id(
    delay, 
    self._timer_id, 
    "HUB:zwave/scripts/timer_handlers/timeout/set_thermostat_setback", 
    timer_args)