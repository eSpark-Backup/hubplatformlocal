local self = ...

local next_event_date_time = nil

self:call(
    "for_each_energy_saving_devices", 
    function(info)
        local saving_mode = info.energy_saving_mode_item.value
        local device_next_event = nil
        
        if self:call("utils/is_postponed_saving_mode", saving_mode) then
            device_next_event = self:call("process_postponed_saving_mode", info)
        elseif self:call("utils/is_saving_mode", saving_mode) then
            device_next_event = self:call("process_saving_mode", info)
        end
        
        if device_next_event then
            if not next_event_date_time or self._time_utils:call("date_time_less_or_equal", device_next_event, next_event_date_time) then
                next_event_date_time = device_next_event
            end
        end
    end
)

if next_event_date_time then
    local duration_to_next_event = self:call("utils/get_duration_to_next_event", next_event_date_time)
    self:call("init_next_timer", duration_to_next_event)
end
