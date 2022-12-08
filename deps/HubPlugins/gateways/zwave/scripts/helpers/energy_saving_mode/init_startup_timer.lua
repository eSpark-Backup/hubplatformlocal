local self = ...

local nodes_ids = self._zwave.get_all_nodes_ids()

if self._timer.exists(self._startup_timer_name) then
    self._timer.cancel(self._startup_timer_name)
end

self._timer.set_timeout_with_id(
    #nodes_ids * 10000, 
    self._startup_timer_name, 
    "HUB:zwave/scripts/timer_handlers/timeout/process_thermostats_energy_saving_modes", 
    {}
)
