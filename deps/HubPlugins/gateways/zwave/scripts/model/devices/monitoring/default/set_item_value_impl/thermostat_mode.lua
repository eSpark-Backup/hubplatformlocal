local self, item_descriptor, value = ...

local delay = 500
local zwave = require("zwave")
local node_info = zwave.get_node_info(item_descriptor.node_id)
if node_info and node_info.command_span > 0 then
    delay = delay + node_info.command_span
end

self:call("set_item_value_impl/_thermostat_mode_set", item_descriptor, value)

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start", delay)

local thermostat_mode_update_timer = hubplugins.object("thermostat_mode_update_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id)

thermostat_mode_update_timer:call("stop")
thermostat_mode_update_timer:call("start")
