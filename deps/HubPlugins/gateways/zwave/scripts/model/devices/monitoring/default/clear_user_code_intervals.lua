local self, node_id, channel_id, user_id = ...

user_id = tostring(math.floor(user_id))

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local item_names = require("HUB:zwave/scripts/resources/item_names")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local core = require "core"

local function clear_intervals_impl(item_name, item_tag)
    local item_id = storage:call("find_item_id", item_name, node_id, class_ids.schedule_entry_lock, channel_id, item_tag)

    if item_id and core.get_item_dictionary_value(item_id, user_id) then
        local slots_type = item_tag or item_name

        print("Request removing user code restrinctions " .. slots_type .. " for user " .. user_id)

        local user_code_intervals_modifier = hubplugins.object(
                                                "sequent_requests_sender/user_code_intervals", 
                                                "HUB:zwave/scripts/utils",
                                                item_id, 
                                                node_id, 
                                                channel_id,
                                                slots_type)

        user_code_intervals_modifier:call("add_requests", { user_id })
    end
end

clear_intervals_impl(item_names.weekly_user_code_intervals, "week_day_per_slot")
clear_intervals_impl(item_names.weekly_user_code_intervals)
clear_intervals_impl(item_names.daily_user_code_intervals)
