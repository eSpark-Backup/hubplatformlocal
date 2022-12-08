local self, item_descriptor, value, item_id = ...

local zwave = require("zwave")

zwave.set_value(item_descriptor.node_id, item_descriptor.class_id, item_descriptor.channel_id, value)

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local node = zwave.get_node(item_descriptor.node_id)
local timeout = 300

-- Request all device channels that support Binary Switch CC
-- todo: reimplement via sequent_requests_sender
for _, channel in ipairs(node.channels) do
    if select(2, find_if(channel.classes, function(cc)
        return cc.id == item_descriptor.class_id
    end)) then
        request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, channel.id, item_descriptor.class_id)
        request_value_timer:call("start", timeout)
        timeout = timeout + 500
    end
end
