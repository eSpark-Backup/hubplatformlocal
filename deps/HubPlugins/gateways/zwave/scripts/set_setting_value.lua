local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local descriptor = storage:call("find_setting_descriptor", params.setting_id)

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(descriptor.node_id)
dev_handler:call("set_setting_value", params.setting_id, params.value)
