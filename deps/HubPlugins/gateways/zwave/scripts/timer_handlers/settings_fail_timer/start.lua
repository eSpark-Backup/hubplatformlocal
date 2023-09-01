local self, value, check_only = ...

local delay = 30000

self._timer.cancel(self._timer_id)

self._timer.set_timeout_with_id(
        delay, 
        self._timer_id, 
        "HUB:zwave/scripts/timer_handlers/timeout/set_failed_setting_status", 
        { 
            setting_id = self._setting_id,
            value = value
        })
