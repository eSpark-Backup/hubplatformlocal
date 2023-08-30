local self, item_descriptor, value, item_id = ...

local core = require("core")

core.update_item_value(item_id, value)

local energy_saving_mode_handler = hubplugins.object("energy_saving_mode", "HUB:zwave/scripts/helpers")
local time_utils = hubplugins.object("time_utils", "HUB:zwave/scripts/utils")

if energy_saving_mode_handler:call("utils/is_postponed_saving_mode", value) then
    local postponed_end_date_time_string = core.calculate_time_with_delay("08:00:00")
    if postponed_end_date_time_string then
        local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
        storage:call(
            "set_postponed_state_end_time", 
            item_id, 
            time_utils:call("parse_date_time", postponed_end_date_time_string) 
        )
    end
end

energy_saving_mode_handler:call("init_next_timer", 3000)
