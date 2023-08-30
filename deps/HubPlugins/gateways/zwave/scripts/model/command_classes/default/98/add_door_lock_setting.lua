local self, inclusion_context, device_node_data, setting_tag, capability = ...

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

if not hubplugins.device_settings_enabled then
    return
end

local setting_info = self:call("settings/" .. setting_tag, capability)

local setting = {
    label = {
        text = setting_info.name
    },
    description = {
        text = setting_info.description
    },
    value_type = setting_info.value_type,
    value = setting_info.value,
    enum = setting_info.enum,
    descriptor = {
        node_id = inclusion_context.zwave_node.id,
        class_id = class_ids.door_lock,
        tag = setting_tag
    }
}

table.insert(device_node_data.settings, setting)
