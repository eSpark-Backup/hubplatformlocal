local self, slot, value, node_id, channel_id = ...

local zwave = require "zwave"

local zwave_value

if value then
    local date_time_from_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/date_time_from_string")

    zwave_value = {}

    zwave_value.startYear, 
    zwave_value.startMonth, 
    zwave_value.startDay, 
    zwave_value.startHour, 
    zwave_value.startMinute =
        date_time_from_string(value.startDateTime)

    zwave_value.stopYear, 
    zwave_value.stopMonth, 
    zwave_value.stopDay, 
    zwave_value.stopHour, 
    zwave_value.stopMinute =
        date_time_from_string(value.stopDateTime)
end

assert(self._node_id)
assert(self._channel_id)
assert(self._dictionary_id)

zwave.set_year_day_entry_lock_schedule(
    self._node_id, 
    self._channel_id, 
    tonumber(self._dictionary_id),
    slot,
    zwave_value)

zwave.request_year_day_entry_lock_schedule(
    self._node_id, 
    self._channel_id, 
    tonumber(self._dictionary_id),
    slot)
