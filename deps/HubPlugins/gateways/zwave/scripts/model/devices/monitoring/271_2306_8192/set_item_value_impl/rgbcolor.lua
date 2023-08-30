local self, item_descriptor, value = ...

item_descriptor.value = value

hubplugins.weak_loadfile("HUB:zwave/scripts/set_rgb_value")(item_descriptor)

local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", item_descriptor.node_id, item_descriptor.channel_id, item_descriptor.class_id)
request_value_timer:call("start")
