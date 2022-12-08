local node_id = ...

local core = require("core")

local storage_helper = hubplugins.object("storage_helper", "HUB:zigbee/scripts/utils")
local device_id = assert(storage_helper:call("find_device_id", node_id))

local function callback(device)
    core.update_device_status(device.id, "broken")
end

hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/for_each_device")(device_id, callback, core)
