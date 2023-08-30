local self, node_id, channel_id = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local zwave = require("zwave")
local cc_door_lock = zwave.get_command_class(node_id, channel_id, class_ids.door_lock)

if cc_door_lock then
    local request_door_lock_state_timer = hubplugins.object("request_value_timer", 
                                                            "HUB:zwave/scripts/timer_handlers", 
                                                            node_id, 
                                                            channel_id, 
                                                            class_ids.door_lock, 
                                                            "request_door_lock_state")
    request_door_lock_state_timer:call("stop")
    zwave.request_door_lock_state(node_id, channel_id)
else
    print(("[handler.request_door_lock_mode]: door lock command class does not exist. node_id: %g, channel_id: %g"):format(node_id, channel_id))
end
