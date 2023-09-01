local node_id = ...

local core = require("core")

local function callback(device)
    core.update_device_status(device.id, "broken")
end

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_device")(node_id, callback)
