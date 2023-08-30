local self, slot, value, node_id, channel_id = ...

local zwave = require "zwave"

local zwave_value

if value then
    local time_from_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/time_from_string")

    zwave_value = {}

    zwave_value.weekDays = value.weekDays
    zwave_value.startHour, zwave_value.startMinute = time_from_string(value.startTime)

    local stopHour, stopMinute = time_from_string(value.stopTime)

    local startTimePoint = zwave_value.startHour * 60 + zwave_value.startMinute
    local stopTimePoint = stopHour * 60 + stopMinute

    local duration = stopTimePoint >= startTimePoint and stopTimePoint - startTimePoint or 
                     (stopTimePoint + 24 * 60 - startTimePoint)

    zwave_value.durationHour = math.floor(duration / 60)
    zwave_value.durationMinute = duration % 60
end

assert(self._node_id)
assert(self._channel_id)
assert(self._dictionary_id)

zwave.set_daily_repeating_entry_lock_schedule(
    self._node_id, 
    self._channel_id, 
    tonumber(self._dictionary_id),
    slot,
    zwave_value)

zwave.request_daily_repeating_entry_lock_schedule(
    self._node_id, 
    self._channel_id, 
    tonumber(self._dictionary_id),
    slot)
