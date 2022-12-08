local self, energy_saving_mode, local_date_time, device_id = ...

local modes_infos = require("HUB:zwave/scripts/resources/energy_saving_modes")

if not modes_infos[energy_saving_mode] then return end

local last_time_setting = nil
local temp_values = nil

for _, mode_setting in ipairs(modes_infos[energy_saving_mode]) do
    local time = self._time_utils:call("parse_time", mode_setting.time)
    if not last_time_setting or self._time_utils:call("time_less_or_equal", last_time_setting.time, time) then
        last_time_setting = {time = time, temperature = mode_setting.temperature}
    end

    if self._time_utils:call("time_less_or_equal", time, local_date_time) then
        temp_values = mode_setting.temperature
    end
end

if not temp_values and last_time_setting then
    temp_values = last_time_setting.temperature
end

return self:call("utils/normalize_setpoints_values", temp_values, device_id)
