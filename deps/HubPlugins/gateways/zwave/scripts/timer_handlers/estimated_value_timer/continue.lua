local self, params = ...

params.spent_time = params.spent_time + params.interval

self._timer.set_timeout_with_id(params.interval, self._timer_id, self._timer_handler_path, params)
