local self, item_id, value, scale = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")

local item = self._core.get_item(item_id)

if not item then return true end

if item.name ~= item_names.thermostat_setpoint_heating and item.name ~= item_names.thermostat_setpoint_cooling then
    return true
end

if item.show == false then return true end

local info = self:call("utils/get_device_energy_saving_info", item.device_id)

if info then
    local saving_mode = info.energy_saving_mode_item.value
    if self:call("utils/is_saving_mode", saving_mode) then
        local local_time = self._time_utils:call("get_local_date_time")
        local temperatures = self:call("get_current_setpoint_temperatures", saving_mode, local_time, info.dev_id)
        local expected_temperature = nil
        if item.name == item_names.thermostat_setpoint_heating then
            expected_temperature = temperatures.heat
        else
            expected_temperature = temperatures.cool            
        end
        
        if not self:call("utils/is_correct_temperature", {value = value, scale = scale}, expected_temperature) then
            return false
        end
    end
end

return true
