local self, item_descriptor, value, item_id = ...

local zwave = require("zwave")

zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

if item_descriptor.channel_id == 0 then
    self:call("request_endpoint_values", item_descriptor.node_id, item_descriptor.class_id)
else
    hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(item_descriptor)
end
