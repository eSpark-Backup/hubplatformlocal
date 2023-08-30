local self, device_info = ...

local saving_mode = device_info.energy_saving_mode_item.value
local local_time = self._time_utils:call("get_local_date_time")

local end_time = self._storage:call("find_postponed_state_end_time", device_info.energy_saving_mode_item.id)

if not end_time or self._time_utils:call("date_time_less_or_equal", end_time, local_time) then
    local actual_saving_mode = self:call("utils/postponed_to_actual_mode", saving_mode)

    self._storage:call("remove_postponed_state_end_time", device_info.energy_saving_mode_item.id)
    self._core.update_item_value(device_info.energy_saving_mode_item.id, actual_saving_mode)
    device_info.energy_saving_mode_item.value = actual_saving_mode
    return self:call("process_saving_mode", device_info)
else
    return end_time
end
