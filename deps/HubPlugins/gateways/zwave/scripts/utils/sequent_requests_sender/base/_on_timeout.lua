local arguments = ...

require ("HUB:zwave/scripts/helpers/scripts_preload")

assert(not arguments.postponed, "timeout of base postponed timer is unexpected")

if arguments.base_postponed_timer_id then
    arguments.timer_id = arguments.timer_id or arguments.timerId
    assert(arguments.timer_id)

    local timer = require "timer"
    local base_arguments = timer.get_arguments(arguments.base_postponed_timer_id)

    if base_arguments then
        print(hubplugins.dump(base_arguments.requests))

        local remove_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/remove_from_array_if")
        remove_if(arguments.requests, function(id)
                return base_arguments.requests[id] ~= arguments.timer_id
            end)

        if #arguments.requests then
            for _, id in ipairs(arguments.requests) do
                base_arguments.requests[id] = nil
            end

            if next(base_arguments.requests) then
                timer.reset_arguments(arguments.base_postponed_timer_id, base_arguments)
            else
                timer.cancel(arguments.base_postponed_timer_id)
            end

            local self = hubplugins.object(table.unpack(base_arguments.creator))
            self:call("_add_requests_impl", arguments.requests)
        end
    end

else
    if arguments.current_request then
        print("timeout of respond waiting for request " .. tostring(arguments.current_request))
    end

    arguments.current_request = arguments.requests[1]
    if arguments.current_request then
        table.remove(arguments.requests, 1)

        local self = hubplugins.object(table.unpack(arguments.creator))
        self._timer.set_timeout_with_id(self._delay, self._timer_id, self._timer_handler_path, arguments) 
        self:call("_request", arguments.current_request)
    end
end
