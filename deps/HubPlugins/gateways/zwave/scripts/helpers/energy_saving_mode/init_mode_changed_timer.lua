local self = ...

if self._timer.exists(self._startup_timer_name) then
    print("Waiting for startup energy saving mode timer, no need to init one more")
    return
end

if self._timer.exists(self._mode_changed_timer_name) then
    self._timer.cancel(self._mode_changed_timer_name)
end

self._timer.set_timeout_with_id(
    5000, 
    self._mode_changed_timer_name, 
    "HUB:zwave/scripts/timer_handlers/timeout/process_thermostats_energy_saving_modes", 
    {}
)
