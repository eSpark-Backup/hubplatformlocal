local self, node_id = ...

local zwave = require( "zwave" )

local device_info = self:call("get_device_info")

if device_info.hail_producing_command_classes then
    for _, request_info in ipairs(device_info.hail_producing_command_classes) do
        zwave.request_value(node_id, request_info.class_id, request_info.channel_id)
    end
else
    local class_ids = require("HUB:zwave/scripts/resources/class_ids")

    local zwave_node = zwave.get_node( node_id )

    local unused_cc_set = {
        [class_ids.hail] = true,
        [class_ids.basic] = true,
    }

    for _, channel in ipairs(zwave_node.channels) do
        for _, cc in ipairs(channel.classes) do
            if not unused_cc_set[cc.id] then
                zwave.request_value(node_id, cc.id, channel.id)
            end
        end
    end
end
