local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(params.node_id)
dev_handler:call("update_value_in_core", params.item_name, params.item_id, params.value)
