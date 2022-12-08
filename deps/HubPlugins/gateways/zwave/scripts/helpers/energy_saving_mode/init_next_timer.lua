local self, duration_to_next_event = ...

if self:call("is_waiting_for_high_priority_timer") then
    print("Waiting for another energy saving mode timer")
    return
end

if self._timer.exists(self._next_event_timer_name) then
    self._timer.cancel(self._next_event_timer_name)
end

self._timer.set_timeout_with_id(
    duration_to_next_event, 
    self._next_event_timer_name, 
    "HUB:zwave/scripts/timer_handlers/timeout/process_thermostats_energy_saving_modes", 
    {}
)
