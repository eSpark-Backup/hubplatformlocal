local self, item_descriptor, value = ...

self:call("set_item_value_impl/_thermostat_mode_set", item_descriptor, value)

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start", 2000)
