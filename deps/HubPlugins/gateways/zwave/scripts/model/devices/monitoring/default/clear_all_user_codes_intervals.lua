local self, node_id, channel_id = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local core = require "core"

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local function clear_intervals_impl(item_name, item_tag)
    local item_id = storage:call("find_item_id", item_name, node_id, class_ids.schedule_entry_lock, channel_id, item_tag)

    if item_id then
        local slots_type = item_tag or item_name
        print("Request clear all user code restrictions " .. slots_type)

        local user_code_intervals_modifier = hubplugins.object(
                                                "sequent_requests_sender/user_code_intervals", 
                                                "HUB:zwave/scripts/utils",
                                                item_id, 
                                                node_id, 
                                                channel_id,
                                                slots_type)

        local item = core.get_item(item_id)
        user_code_intervals_modifier:call("add_slots_clear_requests_by_item", item)
    end
end

clear_intervals_impl(item_names.weekly_user_code_intervals, "week_day_per_slot")
clear_intervals_impl(item_names.weekly_user_code_intervals)
clear_intervals_impl(item_names.daily_user_code_intervals)
