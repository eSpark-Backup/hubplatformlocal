local self, node_id, channel_id, colors, target_colors, target_durations = ...

local transition_done = self:_super_call("default", "update_rgb_value", node_id, channel_id, colors, target_colors, target_durations)

if transition_done then 
    local dimmer_class_id = require("HUB:zwave/scripts/resources/class_ids").dimmer

    local delay = 0
    for channel_id = 2, 5 do
        local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel_id, dimmer_class_id)
        request_value_timer:call("start", delay)

        delay = delay + 500
    end
end

return transition_done
