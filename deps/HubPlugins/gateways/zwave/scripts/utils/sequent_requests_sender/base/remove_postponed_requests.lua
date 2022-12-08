local self, ids = ...

local arguments = self._timer.get_arguments(self._postponed_timer_id)

if arguments then
    local modified

    for _, id in pairs(ids) do
        if arguments.requests[id] then
            arguments.requests[id] = nil
            modified = true
        end
    end

    if modified then
        if next(arguments.requests) then
            self._timer.reset_arguments(self._postponed_timer_id, arguments)
        else
            self._timer.cancel(self._postponed_timer_id)
        end
    end
end
