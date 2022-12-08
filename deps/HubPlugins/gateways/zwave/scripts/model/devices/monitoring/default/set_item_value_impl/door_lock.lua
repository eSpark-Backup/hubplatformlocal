local self, item_descriptor, value = ...

local zwave = require("zwave")

zwave.set_door_lock_mode(item_descriptor.node_id, item_descriptor.channel_id, value)

local request_door_lock_state_timer = hubplugins.object("request_value_timer", 
                                                        "HUB:zwave/scripts/timer_handlers", 
                                                        item_descriptor.node_id, 
                                                        item_descriptor.channel_id, 
                                                        item_descriptor.class_id,
                                                        "request_door_lock_state")

request_door_lock_state_timer:call("start", self:call("set_item_value_impl/_get_door_lock_state_delay"))
