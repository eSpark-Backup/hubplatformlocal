local self, ids = ...

local arguments = self._timer.get_arguments(self._timer_id)

if arguments then
    local find_in_array = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array")

    local modified
    for _, id in ipairs(ids) do
        local index = find_in_array(arguments.requests, id)
        if index then
            table.remove(arguments.requests, index)
            modified = true
        end
    end

    if modified then
        self._timer.reset_arguments(self._timer_id, arguments)
    end
end
