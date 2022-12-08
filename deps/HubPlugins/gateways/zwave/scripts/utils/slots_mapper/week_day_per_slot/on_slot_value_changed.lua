local self, slot, value = ...

self:call("_normalize_values", { value })

if not self._device_supports_shifted_days and value and value.startTime > value.stopTime then
    value.stopTime = value.startTime -- what to do with this??
end

self:_super_call("slots_mapper/base", "on_slot_value_changed", slot, value)
