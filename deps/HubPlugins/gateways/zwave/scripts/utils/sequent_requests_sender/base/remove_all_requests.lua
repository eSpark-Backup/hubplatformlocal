local self = ...

local arguments = self._timer.get_arguments(self._timer_id)

if arguments and arguments.requests[1] then
    arguments.requests = {}
    self._timer.reset_arguments(self._timer_id, arguments)
end

self:call("remove_all_postponed_requests")
