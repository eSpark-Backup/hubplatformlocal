local self, node_id, channel_id, event_type, user_id = ...

local zwave = require("zwave")

if event_type == "power_has_been_applied" then
    zwave.synchronize_device_time(node_id)
end
