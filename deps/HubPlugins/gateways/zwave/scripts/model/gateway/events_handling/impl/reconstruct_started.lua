local params = ...

local gateway_monitoring_handler = hubplugins.object( "monitoring", "HUB:zwave/scripts/model/gateway" )
gateway_monitoring_handler:call("set_setting_status", "zwave.zwave_reconstruct", "pending")
