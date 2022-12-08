local self, node_id, channel_id, class_id, value, target_value, duration = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if class_id == class_ids.switch_binary and channel_id ~= 0 then
    hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(
        {
            node_id = node_id,
            class_id = class_ids.switch_binary,
            channel_id = 0
        }
    )
end

self:_super_call("default", "update_value", node_id, channel_id, class_id, value, target_value, duration)
