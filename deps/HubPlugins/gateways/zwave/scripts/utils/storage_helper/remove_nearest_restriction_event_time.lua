local self = ...

if self._storage.exists("zwave_virtual_pin_code_restriction_nearest_event_time") then
    self._storage.delete("zwave_virtual_pin_code_restriction_nearest_event_time")
end
