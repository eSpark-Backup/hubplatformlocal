local self, timer_id, delay, postponed_timer_delay = ...

self:_register("sequent_requests_sender/base", "HUB:zwave/scripts/utils")

self._delay = delay or 10000
self._postponed_timer_delay = postponed_timer_delay or 5000
self._timer_id = timer_id
self._postponed_timer_id = timer_id .. "_postponed"
self._timer_handler_path = "HUB:zwave/scripts/utils/sequent_requests_sender/base/_on_timeout"
self._timer = assert(require("timer"), "Fail to load the 'timer' functionality")
