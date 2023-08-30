local self = ...

return self._timer.exists(self._startup_timer_name) 
    or self._timer.exists(self._mode_changed_timer_name)
