local self, item_descriptor, value, item_id = ...

local zwave = require("zwave")

zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start", 1000)

if item_descriptor.channel_id == 0 then
    self:call("request_endpoint_values", item_descriptor.node_id, item_descriptor.class_id, 1500)
end
