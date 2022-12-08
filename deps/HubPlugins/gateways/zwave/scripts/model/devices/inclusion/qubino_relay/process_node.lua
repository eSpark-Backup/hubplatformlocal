local self, zwave_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")

-- Temperature sensor from channel 0 should be disabled
local _, channel_0 = find_if(zwave_node.channels, function(channel) return channel.id == 0 end)
if channel_0 then
    local _, cc = find_if(channel_0.classes, function(cc) return cc.id == 49 end)
    if cc then
        cc.sensors = {}
    end
end

return self:_super_call("default", "process_node", zwave_node)
