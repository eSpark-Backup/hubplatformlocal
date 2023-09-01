local self, zwave_node = ...

local zwave = require("zwave")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")
local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")

-- If device is included with only 1 channel, we should set lifeline association without targetChannelId to make device report values
if #zwave_node.channels == 1 then
    zwave.remove_node_from_association_group(zwave_node.id, 0, 1, 1, 0)
    zwave.add_node_to_association_group(zwave_node.id, 0, 1, 1)
end

-- Disable Basic CC functionality to avoid switch device duplication
for _, channel in ipairs(zwave_node.channels) do
    if channel.classes then
        local _, cc = find_if(channel.classes, function(cc) return cc.id == class_ids.basic end)
        if cc then
            cc.is_supported = false
        end
    end
end

return self:_super_call("qubino_relay", "process_node", zwave_node)
