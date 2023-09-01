local self, node_id, channel_id, class_id, user_id, slot_id, value = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local item_id = self:call("find_item_id", item_names.daily_user_code_intervals, node_id, channel_id, class_id)
local user_code_intervals_modifier = hubplugins.object(
                                        "sequent_requests_sender/user_code_intervals", 
                                        "HUB:zwave/scripts/utils",
                                        item_id, 
                                        node_id, 
                                        channel_id,
                                        item_names.daily_user_code_intervals)

local date_time_to_string = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/date_time_to_string")

user_code_intervals_modifier:call("on_slot_report", user_id, slot_id, value and {
    startDateTime = date_time_to_string(value.startYear, value.startMonth, value.startDay, value.startHour, value.startMinute, 0),
    stopDateTime = date_time_to_string(value.stopYear, value.stopMonth, value.stopDay, value.stopHour, value.stopMinute, 0)
})
