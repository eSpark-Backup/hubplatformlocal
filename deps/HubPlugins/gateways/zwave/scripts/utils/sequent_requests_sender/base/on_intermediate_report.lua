local self, id = ...

local arguments = self._timer.get_arguments(self._timer_id)

if arguments and id == arguments.current_request then
    self._timer.reset(self._delay, self._timer_id)
end
