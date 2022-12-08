local self, node_id, setting_descriptor, key, old_value, new_value = ...

local zwave = assert(require("zwave"), "Failed to load zwave functionality")
local helper = hubplugins.object("associations_helper", "HUB:zwave/scripts/helpers")

local function get_association_info_key(association_info)
    local key = tostring(math.floor(association_info.targetNode))
    if association_info.targetChannel then
        key = key .. "_" .. tostring(math.floor(association_info.targetChannel))
    end

    return key
end

local function associations_to_map(values, predicate)
    local result = {}
    if values then
        for _, association_info in ipairs(values) do
            if not predicate or predicate(association_info) then
                local key = get_association_info_key(association_info)
                result[key] = association_info
            end
        end
    end

    return result
end

-- main

local channel_id, group_id = helper:call("identifier_to_group", key)
if not channel_id or not group_id then
    print("Unsupported association key: " .. key)
    return
end

local info_setting = self:call("find_setting", node_id, nil, "associations_groups")
if not info_setting or not info_setting.value[key] then
    print(string.format("Association group is not supported, node id: %g, channel id: %g, group id: %g", node_id, channel_id, group_id))
    return
end

local associations_max_count = assert(info_setting.value[key].maxAssociationsNumber)
if #new_value > associations_max_count then
    print(string.format("Association group is not supported, node id: %g, channel id: %g, group id: %g, max_nodes_count: %g",
        node_id,
        channel_id,
        group_id,
        associations_max_count))
    return
end

local old_value_associations = associations_to_map(old_value)
local new_value_associations = associations_to_map(new_value, function(association_info)
    if association_info.targetNode == 1 or zwave.has_node(association_info.targetNode) then
        return true
    else
        print("Trying to add unknown node to association group, node id: " .. association_info.targetNode)
        return false
    end
end)

for key, association_info in pairs(old_value_associations) do
    if not new_value_associations[key] then
        zwave.remove_node_from_association_group(
            node_id,
            channel_id,
            group_id,
            association_info.targetNode,
            association_info.targetChannel
        )
    end
end

for key, association_info in pairs(new_value_associations) do
    if not old_value_associations[key] then
        zwave.add_node_to_association_group(
            node_id,
            channel_id,
            group_id,
            association_info.targetNode,
            association_info.targetChannel
        )
    end
end

zwave.request_association_group(node_id, channel_id, group_id)
