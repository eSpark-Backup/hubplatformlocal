local self, node_id, class_id, predicate = ...

local zwave = require("zwave")

local zwave_node = zwave.get_node(node_id)

for _, channel in ipairs(zwave_node.channels) do
    for _, cc in ipairs(channel.classes) do
        if cc.id == class_id then
            if predicate(channel) then
                return channel
            end

            break
        end
    end
end
