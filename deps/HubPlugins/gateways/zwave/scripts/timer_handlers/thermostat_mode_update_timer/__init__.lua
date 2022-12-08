local self, node_id, channel_id = ...

self:_register("thermostat_mode_update_timer", "HUB:zwave/scripts/timer_handlers")

self._node_id = node_id
self._channel_id = channel_id
self._timer_id = math.floor(node_id) .. "_" .. math.floor(channel_id) .. "_thermostat_mode_update_timer"
self._timer = require "timer"
