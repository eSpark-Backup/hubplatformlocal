local self, zwave_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

-- Remove schedule entry lock command class
local _, channel_0 = find_if(zwave_node.channels, function(channel) return channel.id == 0 end)
if channel_0 then
    local pos, cc = find_if(channel_0.classes, function(cc) return cc.id == class_ids.schedule_entry_lock end)
    if cc then
        table.remove(channel_0.classes, pos)
    end
end

return self:_super_call("default", "process_node", zwave_node)
