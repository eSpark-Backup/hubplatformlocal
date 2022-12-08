local self, slot, value = ...

if value ~= nil then
    hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/sort_week_days")(value.weekDays)
end

self:_super_call("slots_mapper/base", "on_slot_value_changed", slot, value)
