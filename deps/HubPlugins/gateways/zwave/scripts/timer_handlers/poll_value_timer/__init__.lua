local self, node_id, channel_id, class_id, method --[[opt]] = ...

self:_register("poll_value_timer", "HUB:zwave/scripts/timer_handlers")
self._node_id = node_id
self._channel_id = channel_id
self._class_id = class_id
self._method = method

self._timer_id = table.concat( { "polling", method or "request_value", math.floor(node_id), math.floor(channel_id), math.floor(class_id or 0) }, "_" )
self._timer = require "timer"
