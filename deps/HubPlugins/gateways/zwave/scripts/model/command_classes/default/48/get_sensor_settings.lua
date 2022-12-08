local self, inclusion_context, build_tree_context, device_node, item_name = ...

local item_names = require("HUB:zwave/scripts/resources/item_names")
local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local item_name_to_settings_tags = {
    [item_names.tilt] = {"tilt_alarm"}
}

local settings = {}

if item_name_to_settings_tags[item_name] ~= nil then
    for _, setting_tag in ipairs(item_name_to_settings_tags[item_name]) do
        local setting = self:call("settings/" .. setting_tag)
        setting.descriptor = {
            node_id = inclusion_context.zwave_node.id,
            class_id = class_ids.sensor_binary,
            channel_id = build_tree_context.channel.id,
            tag = setting_tag
        }
        table.insert(settings, setting)
    end
end

return #settings ~= 0 and settings or nil
