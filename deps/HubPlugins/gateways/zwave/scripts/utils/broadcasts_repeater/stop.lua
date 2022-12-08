local self, event_name = ...

local timer_id = self:call("_generate_timer_id", event_name)

self._timer.cancel(timer_id)
