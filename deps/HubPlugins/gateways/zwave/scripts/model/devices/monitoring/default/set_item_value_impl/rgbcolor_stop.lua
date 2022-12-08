local self, item_descriptor, value, item_id = ...

local zwave = require("zwave")

if value == "all" then
    local core = require "core"
    local item = core.get_item(item_id)

    for _, component in pairs(item.enum) do
        if component ~= value then
            zwave.color_switch_level_change(item_descriptor.node_id, item_descriptor.channel_id, component, "stop")
        end
    end
else
    zwave.color_switch_level_change(item_descriptor.node_id, item_descriptor.channel_id, value, "stop")
end

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start", 500)
