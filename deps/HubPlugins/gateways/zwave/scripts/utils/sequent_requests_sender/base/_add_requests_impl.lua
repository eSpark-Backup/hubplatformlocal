local self, ids = ...

local arguments = self._timer.get_arguments(self._timer_id)

local arguments_modified
local arguments_created

local find_in_array = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array")

for _, id in ipairs(ids) do
    if not arguments then
        arguments = {
            creator = self:call("_get_self_creator"),
            current_request = id,
            requests = { }
        }
        arguments_created = true
    elseif arguments.current_request ~= id and arguments.requests[id] == nil then
        local index = find_in_array(arguments.requests, id)

        if index ~= #arguments.requests then
            if index then
                table.remove(arguments.requests, index)
            end

            table.insert(arguments.requests, id)
            arguments_modified = true
        end
    end
end

if arguments_created then
    self._timer.set_timeout_with_id(self._delay, self._timer_id, self._timer_handler_path, arguments)
    self:call("_request", arguments.current_request)
elseif arguments_modified then
    self._timer.reset_arguments(self._timer_id, arguments)
end
