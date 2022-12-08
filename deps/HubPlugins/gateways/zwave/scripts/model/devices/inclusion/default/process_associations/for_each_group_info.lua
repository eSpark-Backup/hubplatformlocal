local self, zwave_node, callback = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local function find_association_group(channel, group_id, cc_id)
    local _, cc = find_if(channel.classes, function(cc) return cc.id == cc_id end)
    if cc and cc.association_groups then
        local _, group_info = find_if(cc.association_groups, function(group) return group.group_id == group_id end)
        return group_info
    end
end

local function find_group_max_nodes_count(channel, group_id)
    local group = find_association_group(channel, group_id, class_ids.multi_channel_associations)
    if not group then
        group = find_association_group(channel, group_id, class_ids.associations)
    end

    if group then return group.group_nodes_max_count end
end

if zwave_node.channels then
    for _, channel in ipairs(zwave_node.channels) do
        local _, cc = find_if(channel.classes, function(cc) return cc.id == class_ids.association_group_info end)
        if cc and cc.association_groups then
            for _, assoc_group in ipairs(cc.association_groups) do
                local max_nodes_count = find_group_max_nodes_count(channel, assoc_group.group_id)
                if max_nodes_count then
                    callback(channel.id, assoc_group, max_nodes_count)
                end
            end
        end
    end
end
