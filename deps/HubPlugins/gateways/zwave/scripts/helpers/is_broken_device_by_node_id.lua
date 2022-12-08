local node_id = ...

local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local device_id = storage:call("find_device_id", node_id)

local core = require("core")
local device = core.get_device(device_id)

return device.status == "broken"
