local self, interval = ...

interval = interval or 1000

if self._timer.exists(self._timer_id) then
    self._timer.reset(interval, self._timer_id)
else
    local timer_args = {
        node_id = self._node_id,
        channel_id = self._channel_id,
        class_id = self._class_id,
        method = self._method
    }

    self._timer.set_interval_with_id(interval, self._timer_id, "HUB:zwave/scripts/timer_handlers/common/request_value", timer_args)
end
