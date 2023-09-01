local self, predicate = ...

local arguments = self._timer.get_arguments(self._postponed_timer_id)

if arguments then
    local to_be_removed = {}
    for id in pairs(arguments.requests) do
        if predicate(id) then
            table.insert(to_be_removed, id)
        end
    end

    if #to_be_removed then
        for _, id in ipairs(to_be_removed) do
            arguments.requests[id] = nil
        end
        
        if next(arguments.requests) then
            self._timer.reset_arguments(self._postponed_timer_id, arguments)
        else
            self._timer.cancel(self._postponed_timer_id)
        end
    end
end
