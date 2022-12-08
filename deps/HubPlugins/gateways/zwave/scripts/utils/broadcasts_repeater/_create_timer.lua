local self, timer_id, broadcast_body, attempt = ...

if timer_id then
    self._timer.cancel(timer_id)
end

local args = {
    broadcast_body = broadcast_body,
    attempt = attempt,
    creator = {
        "broadcasts_repeater",
        "HUB:zwave/scripts/utils",
        self._delay,
        self._repeats_count
    }
}

self._timer.set_timeout_with_id(self._delay, timer_id, self._timer_handler_path, args)
