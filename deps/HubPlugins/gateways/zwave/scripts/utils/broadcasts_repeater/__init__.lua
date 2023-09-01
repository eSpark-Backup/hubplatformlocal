local self, delay, repeats_count = ...

self:_register("broadcasts_repeater", "HUB:zwave/scripts/utils")

self._delay = delay or 10000
self._repeats_count = repeats_count or 5

self._timer_handler_path = "HUB:zwave/scripts/utils/broadcasts_repeater/_on_timeout"
self._timer = assert(require("timer"), "Fail to load the 'timer' functionality")
self._core = assert(require("core"), "Fail to load the 'core' functionality")
