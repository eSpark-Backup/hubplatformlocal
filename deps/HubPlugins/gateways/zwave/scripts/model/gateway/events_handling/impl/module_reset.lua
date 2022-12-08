local params = ...

local status = params.status

local to_setting_status = {
    invoked = "pending",
    started = "pending",
    failed = "failed",
    timeout = "failed",
    finished = "synced"
}

local setting_status = to_setting_status[status]

local gateway_monitoring_handler = hubplugins.object( "monitoring", "HUB:zwave/scripts/model/gateway" )

if status == "finished" then
    gateway_monitoring_handler:call("init_network_status_settings_values")
end

if setting_status then
    gateway_monitoring_handler:call("set_setting_status", "zwave.zwave_reset", setting_status)
end
