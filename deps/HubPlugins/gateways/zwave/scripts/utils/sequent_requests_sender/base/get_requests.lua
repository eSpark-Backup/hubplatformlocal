local self = ...

local arguments = self._timer.get_arguments(self._timer_id)

if arguments then
    return arguments.requests, arguments.current_request
end
