local self, value, target_value, target_duration, spent_time, interval = ...

interval = interval or 1000

self._timer.set_timeout_with_id(interval, self._timer_id, self._timer_handler_path, {  
        item_id = self._item_id,
        value = value,
        target_value = target_value,
        target_duration = target_duration,
        spent_time = interval,
        interval = interval
    })