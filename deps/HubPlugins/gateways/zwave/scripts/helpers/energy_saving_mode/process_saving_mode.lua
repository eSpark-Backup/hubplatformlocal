local self, device_info = ...

local saving_mode = device_info.energy_saving_mode_item.value
local local_time = self._time_utils:call("get_local_date_time")

-- Process current temperatures
local temperatures = self:call("get_current_setpoint_temperatures", saving_mode, local_time, device_info.dev_id)
if temperatures then
    self:call("set_setpoint_value", device_info.setpoint_heating_item, temperatures.heat)
    self:call("set_setpoint_value", device_info.setpoint_cooling_item, temperatures.cool)
end

-- Calculate next event
return self:call("get_next_saving_mode_event", saving_mode, local_time)
