local self = ...

local arguments = self._timer.get_arguments(self._postponed_timer_id)

if arguments then
    local result = { }
    for id in pairs( arguments.requests ) do
        result[id] = true
    end
    
    return result
end
