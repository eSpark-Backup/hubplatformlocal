local params = ...

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)

dev_handler:call("set_setting_status", params.node_id, params.parameter_num, params.status)
