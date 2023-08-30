local self, node_id, channel_id, class_id, user_id, slot_id, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_id = self:call("find_item_id", item_names.weekly_user_code_intervals, node_id, channel_id, class_id)

local user_code_intervals_modifier = hubplugins.object(
                                        "sequent_requests_sender/user_code_intervals", 
                                        "HUB:zwave/scripts/utils",
                                        item_id, 
                                        node_id, 
                                        channel_id,
                                        item_names.weekly_user_code_intervals)

local core_value

if value ~= nil then
    local time_to_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/time_to_string")

    local startTimePoint = value.startHour * 60 + value.startMinute
    local stopTimePoint = (startTimePoint + value.durationHour * 60 + value.durationMinute) % (24 * 60)

    local stopHour = math.floor(stopTimePoint / 60);
    local stopMinute = stopTimePoint % 60

    core_value = {
        weekDays = value.weekDays,
        startTime = time_to_string(value.startHour, value.startMinute, 0),
        stopTime = time_to_string(stopHour, stopMinute, 0),
    }
end

user_code_intervals_modifier:call("on_slot_report", user_id, slot_id, core_value)
