local self, slot, value, node_id, channel_id = ...

local zwave = require "zwave"

local zwave_value

if value then
    local time_from_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/time_from_string")

    zwave_value = {}

    zwave_value.startHour, zwave_value.startMinute = time_from_string(value.startTime)
    zwave_value.stopHour, zwave_value.stopMinute = time_from_string(value.stopTime)
    zwave_value.weekDay = value.weekDay
end

assert(self._node_id)
assert(self._channel_id)
assert(self._dictionary_id)

zwave.set_week_day_entry_lock_schedule(
    self._node_id, 
    self._channel_id, 
    tonumber(self._dictionary_id),
    slot,
    zwave_value)

zwave.request_week_day_entry_lock_schedule(
    self._node_id, 
    self._channel_id, 
    tonumber(self._dictionary_id),
    slot)
