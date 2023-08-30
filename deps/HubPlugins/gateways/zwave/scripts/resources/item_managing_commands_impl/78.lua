local item = ...
local item_names = require("HUB:zwave/scripts/resources/item_names")
-- Schedule Entry Lock
local commands = {
    [item_names.weekly_user_code_intervals] = {"SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET"},
    [item_names.daily_user_code_intervals] = {"SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_SET"},
}

return commands[item.descriptor.item_name]
