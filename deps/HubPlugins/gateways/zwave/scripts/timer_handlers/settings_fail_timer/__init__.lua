local self, setting_id = ...

self:_register("settings_fail_timer", "HUB:zwave/scripts/timer_handlers")

self._setting_id = setting_id
self._timer_id = setting_id .. "_settings_fail_timer"
self._timer = require "timer"
