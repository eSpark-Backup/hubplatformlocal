local self, new_values, should_process_commands = ...

local sort_week_days = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/sort_week_days")

if new_values ~= nil then
    for _, value in ipairs(new_values) do
        sort_week_days(value.weekDays)
    end
end

return self:_super_call("slots_mapper/base", "set_values", new_values, should_process_commands)
