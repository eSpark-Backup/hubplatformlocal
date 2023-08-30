local self, ids = ...

local arguments = self._timer.get_arguments(self._postponed_timer_id)

local secondary_timer_id = self._timer.set_timeout(
    self._postponed_timer_delay, 
    self._timer_handler_path, 
    { 
        base_postponed_timer_id = self._postponed_timer_id,
        requests = ids
    })

for _, id in ipairs(ids) do
    if not arguments then
        arguments = {
            postponed = true,
            creator = self:call("_get_self_creator"),
            requests = {}
        }
    end

    arguments.requests[id] = secondary_timer_id
end

self._timer.cancel(self._postponed_timer_id)

local timer_callback_sending_max_delay = 10000
self._timer.set_timeout_with_id(self._postponed_timer_delay + timer_callback_sending_max_delay, self._postponed_timer_id, self._timer_handler_path, arguments)

