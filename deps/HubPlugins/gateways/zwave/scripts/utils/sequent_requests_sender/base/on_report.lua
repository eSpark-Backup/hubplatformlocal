local self, ids, handle_unexpected_reports = ...

assert(ids)

ids = type(ids) == "table" and ids or { ids }

if handle_unexpected_reports then
    self:call("remove_postponed_requests", ids)
end

local arguments = self._timer.get_arguments(self._timer_id)

if arguments then
    local requests_count = #arguments.requests
    for _, id in ipairs(ids) do
        if id == arguments.current_request then
            arguments.current_request = nil

        elseif handle_unexpected_reports then
            local find_in_array = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array")
            local index = find_in_array(arguments.requests, id)
            if index then
                table.remove(arguments.requests, index)
            end
        end
    end

    if arguments.current_request == nil then
        self._timer.cancel(self._timer_id)
        arguments.current_request = arguments.requests[1]

        if arguments.current_request then
            table.remove(arguments.requests, 1)
            self._timer.set_timeout_with_id(self._delay, self._timer_id, self._timer_handler_path, arguments)
            self:call("_request", arguments.current_request)
        end

    elseif requests_count ~= #arguments.requests then
        self._timer.reset_arguments(self._timer_id, arguments)
    end
end
