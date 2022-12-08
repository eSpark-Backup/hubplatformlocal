local self, item_id = ...

self:_register("estimated_value_timer", "HUB:zwave/scripts/timer_handlers")

self._item_id = item_id
self._timer_id = item_id .. "_estimated_value_timer"
self._timer_handler_path = "HUB:zwave/scripts/timer_handlers/timeout/set_estimated_value"
self._timer = require "timer"