local self, predicate = ...

local arguments = self._timer.get_arguments(self._timer_id)

if arguments then
    local remove_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/remove_from_array_if")

    if remove_if( arguments.requests, predicate ) > 0 then
        self._timer.reset_arguments(self._timer_id, arguments)
    end
end

self:call("remove_postponed_requests_if", predicate)
