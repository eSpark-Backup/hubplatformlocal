local self, setting_id, value, descriptor = ...

-- todo: override this handler for specific thermostats that send report automatically

if self:call("set_setting_value_impl/_thermostat_manufacturer_specific_data_set", setting_id, value, descriptor ) then
    hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(descriptor)
end
