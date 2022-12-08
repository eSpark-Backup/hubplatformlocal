local self, zwave_node = ...

local find_if = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/find_in_array_if")
local helper = hubplugins.object("associations_helper", "HUB:zwave/scripts/helpers")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local setting = self:call(
    "process_associations/get_associations_groups_setting_template",
    zwave_node.id
)

self:call("process_associations/for_each_group_info", zwave_node, function(channel_id, assoc_group, max_nodes_count)
    local setting_group_info = {
        name = assoc_group.group_name,
        channel = channel_id,
        maxAssociationsNumber = max_nodes_count
    }

    if assoc_group.profile and assoc_group.profile.name then
        setting_group_info.profile = assoc_group.profile.name
    end

    if assoc_group.commands then
        for _, group_command in ipairs(assoc_group.commands) do
            if group_command.name then
                if not setting_group_info.commands then setting_group_info.commands = {} end
                table.insert(setting_group_info.commands, group_command.name)
            end
        end
    end

    setting.value[helper:call("group_to_identifier", channel_id, assoc_group.group_id)] = setting_group_info
end)

if next(setting.value) then
    return setting
end
