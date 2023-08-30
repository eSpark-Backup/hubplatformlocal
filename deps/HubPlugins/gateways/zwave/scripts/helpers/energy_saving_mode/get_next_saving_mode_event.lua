local self, energy_saving_mode, local_date_time = ...

local modes_infos = require("HUB:zwave/scripts/resources/energy_saving_modes")

if not modes_infos[energy_saving_mode] then return end

local next_event_date_time = nil

for _, mode_setting in ipairs(modes_infos[energy_saving_mode]) do
    local mode_setting_date_string = self._core.get_next_week_event(self._time_utils:call("get_all_weekdays"), mode_setting.time)
    if mode_setting_date_string then
        local mode_setting_date = self._time_utils:call("parse_date_time", mode_setting_date_string)
        if not next_event_date_time or self._time_utils:call("date_time_less_or_equal", mode_setting_date, next_event_date_time) then
            next_event_date_time = mode_setting_date
        end
    end
end

return next_event_date_time
