local self, node_id, class_id, initial_delay = ...

local delay = initial_delay or 0
self:call("for_each_channel_with_cc", node_id, class_id, function(channel)
    if channel.id ~= 0 then
        local request_value_timer = hubplugins.object("request_value_timer", "HUB:zwave/scripts/timer_handlers", node_id, channel.id, class_id)

        request_value_timer:call("start", delay)
        delay = delay + 500
    end
end)
