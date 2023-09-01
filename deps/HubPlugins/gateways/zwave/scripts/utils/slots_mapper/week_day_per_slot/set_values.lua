local self, new_values, should_process_commands = ...

if new_values ~= nil then
    new_values = self:call("_normalize_values", new_values)
    local sort_week_days = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/sort_week_days")
    local ensure_unique = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/ensure_sorted_elements_unique")

    for _, value in ipairs(new_values) do
        sort_week_days(value.weekDays)
        ensure_unique(value.weekDays)
    end
end

return self:_super_call("slots_mapper/base", "set_values", new_values, should_process_commands)
