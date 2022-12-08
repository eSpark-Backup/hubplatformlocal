local self, node_id, channel_id, class_id, group_id, group_nodes = ...

local core = require "core"
local helper = hubplugins.object("associations_helper", "HUB:zwave/scripts/helpers")

local associations_setting = self:call("find_setting", node_id, nil, "associations")

if not associations_setting then return end

local new_value = {}
if group_nodes then
    for _, node_info in ipairs(group_nodes) do
        table.insert(new_value, { targetNode = node_info.node_id, targetChannel = node_info.endpoint_id })
    end
end

local group_identifier = helper:call("group_to_identifier", channel_id, group_id)
core.set_setting_dictionary_value(associations_setting.id, group_identifier, new_value)
