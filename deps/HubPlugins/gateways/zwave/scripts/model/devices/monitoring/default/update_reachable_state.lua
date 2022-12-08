local self, node_id, value = ...

local core = require("core")

local function callback(device)
    if device.reachable ~= value then
        core.update_reachable_state(device.id, value)
    end
end

hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/for_each_device")(node_id, callback)
