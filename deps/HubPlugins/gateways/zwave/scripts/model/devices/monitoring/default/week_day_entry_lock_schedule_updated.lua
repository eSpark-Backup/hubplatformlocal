local self, node_id, channel_id, class_id, user_id, slot_id, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_id = self:call("find_item_id", item_names.weekly_user_code_intervals, node_id, channel_id, class_id, "week_day_per_slot")
local user_code_intervals_modifier = hubplugins.object(
                                        "sequent_requests_sender/user_code_intervals", 
                                        "HUB:zwave/scripts/utils",
                                        item_id, 
                                        node_id, 
                                        channel_id,
                                        "week_day_per_slot")

local time_to_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/time_to_string")

-- TODO: handle start time point < stop time point

user_code_intervals_modifier:call("on_slot_report", user_id, slot_id, value and {
    weekDay = value.weekDay,
    startTime = time_to_string(value.startHour, value.startMinute, 0),
    stopTime = time_to_string(value.stopHour, value.stopMinute, 0),
})
