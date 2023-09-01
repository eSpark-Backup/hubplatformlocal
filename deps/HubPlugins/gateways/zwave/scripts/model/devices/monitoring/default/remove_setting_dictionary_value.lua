local self, setting_id, node_id, setting_descriptor, key, old_value = ...

local core = assert(require("core"), "Failed to load core functionality")
local zwave = assert(require("zwave"), "Failed to load zwave functionality")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if not setting_descriptor then return end

if setting_descriptor.class_id == class_ids.configuration then
    core.set_setting_dictionary_value(setting_id, key, {})
    zwave.remove_configuration(node_id, old_value.number)

elseif setting_descriptor.class_id == nil and setting_descriptor.tag == "associations" then
    local helper = hubplugins.object("associations_helper", "HUB:zwave/scripts/helpers")
    local channel_id, group_id = helper:call("identifier_to_group", key)
    if not channel_id or not group_id then
        print("Unsupported association key: " .. key)
        return
    end

    if old_value then
        for _, association_info in ipairs(old_value) do
            zwave.remove_node_from_association_group(
                node_id,
                channel_id,
                group_id,
                association_info.targetNode,
                association_info.targetChannel
            )
        end
    end

    zwave.request_association_group(node_id, channel_id, group_id)
end
