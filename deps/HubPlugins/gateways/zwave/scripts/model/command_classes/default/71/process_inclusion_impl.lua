local self, inclusion_context, build_tree_context, device_node = ...

if hubplugins.device_settings_enabled then
    local class_id = require("HUB:zwave/scripts/resources/class_ids").thermostat_setback

    local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

    local function add_setting(setting_tag)
        local setting = self:call("settings/" .. setting_tag)
        setting.descriptor = {
            node_id = inclusion_context.zwave_node.id,
            class_id = class_id,
            tag = setting_tag
        }

        table.insert(device_node_data.settings, setting)
    end

    add_setting("thermostat_setback_value")
    add_setting("thermostat_setback_mode")
    add_setting("thermostat_setback_override_type")
end
