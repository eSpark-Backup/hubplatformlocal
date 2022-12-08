local self, use_fixed_wake_up_interval, wake_up_interval, always_on_devices_inetrval, attempts_count = ...

if not use_fixed_wake_up_interval then
    use_fixed_wake_up_interval = self:call( "get_setting_value", "zwave.reachable_check_policy") ~= "default"
end

if not wake_up_interval then
    wake_up_interval = self:call( "get_setting_value", "zwave.wake_up_devices_polling_interval")
end

if not always_on_devices_inetrval then
    always_on_devices_inetrval = self:call( "get_setting_value", "zwave.mains_devices_polling_interval")
end

if not attempts_count then
    attempts_count = self:call("get_setting_value", "zwave.reachable_check_attempts_count")
end

if use_fixed_wake_up_interval ~= nil and wake_up_interval and always_on_devices_inetrval and attempts_count then
    local zwave = require("zwave")
    zwave.set_devices_poll_intervals(
        use_fixed_wake_up_interval, 
        wake_up_interval, 
        always_on_devices_inetrval, 
        attempts_count
    )
end
