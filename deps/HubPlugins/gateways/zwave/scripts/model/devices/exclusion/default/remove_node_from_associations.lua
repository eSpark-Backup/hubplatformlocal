local self, node_id = ...

local zwave = require("zwave")
local core = require("core")
local storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
local helper = hubplugins.object("associations_helper", "HUB:zwave/scripts/helpers")

local function for_each_node_setting(callback)
    local node_ids = zwave.get_all_nodes_ids()
    for _, node_id_to_check in ipairs(node_ids) do
        local setting_id = storage:call("find_setting_id", node_id_to_check, nil, "associations")
        if setting_id then
            local setting = core.get_setting(setting_id)
            if setting and setting.value then
                callback(node_id_to_check, setting)
            end
        end
    end
end

for_each_node_setting(function (node_id_to_check, setting)
    for key, value in pairs(setting.value) do
        local channel_id, group_id = helper:call("identifier_to_group", key)
        if not channel_id or not group_id then
            print("Unsupported association key: " .. key)
            return
        end

        local removed = false
        for _, association_info in ipairs(value) do
            if tonumber(association_info.targetNode) == tonumber(node_id) then
                removed = true
                zwave.remove_node_from_association_group(
                    node_id_to_check,
                    channel_id,
                    group_id,
                    association_info.targetNode,
                    association_info.targetChannel
                )
            end
        end

        if removed then
            zwave.request_association_group(node_id_to_check, channel_id, group_id)
        end
    end
end)
