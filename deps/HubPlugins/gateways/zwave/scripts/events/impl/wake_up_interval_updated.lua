local params = ...

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("wake_up_interval_updated", params.node_id, params.interval)
