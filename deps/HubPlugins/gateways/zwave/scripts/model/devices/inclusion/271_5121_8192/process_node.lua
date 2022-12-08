local self, zwave_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

-- Disable basic CC in 2nd channel
local _, channel_2 = find_if(zwave_node.channels, function(channel) return channel.id == 2 end)
if channel_2 then
    local _, cc = find_if(channel_2.classes, function(cc) return cc.id == class_ids.basic end)
    if cc then
        cc.is_supported = false
    end
end

return self:_super_call("default", "process_node", zwave_node)
