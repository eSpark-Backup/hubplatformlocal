local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

local device_id = params.device_id
local parent_device_id = nil
local check = params.check or "value"
local items = params.items

print(("Device check executed, device_id: %s, mode: %s"):format(device_id, check))

local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")

local device = assert(core.get_device(device_id))
if device.parent_device_id then
    parent_device_id = device.parent_device_id
end

local node_id = assert(storage:call("find_node_id", parent_device_id ~= "" and parent_device_id or device_id))

local dev_handler = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_handler_monitoring")(node_id)
dev_handler:call("check_device", node_id, device_id, check, items)
