local params = ...

local gateway_monitoring_handler = hubplugins.object( "monitoring", "HUB:zwave/scripts/model/gateway" )

gateway_monitoring_handler:call("init_network_status_settings_values")
