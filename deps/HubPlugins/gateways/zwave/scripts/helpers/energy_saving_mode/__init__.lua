local self = ...

self:_register("energy_saving_mode", "HUB:zwave/scripts/helpers")

self._core = assert(require("core"), "failed to load core")
self._zwave = assert(require("zwave"), "failed to load zwave")
self._timer = assert(require("timer"), "failed to load timer")
self._storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
self._time_utils = hubplugins.object("time_utils", "HUB:zwave/scripts/utils")

self._next_event_timer_name = "zwave_thermostats_energy_saving_timer"
self._startup_timer_name = "zwave_thermostats_energy_saving_startup_timer"
self._mode_changed_timer_name = "zwave_thermostats_energy_saving_mode_changed_timer"
