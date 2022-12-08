local self, inclusion_context, build_tree_context, device_node = ...

if not hubplugins.device_settings_enabled then
    return
end

local class_ids = require("HUB:zwave/scripts/resources/class_ids")

local device_node_data = inclusion_context.device_tree_model:call("get_data", device_node)

local function add_setting(setting_tag)
    local setting_info = self:call("settings/" .. setting_tag)

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
            class_id = class_ids.indicator,
            channel_id = build_tree_context.channel.id,
            tag = setting_tag
        }
    }

    table.insert(device_node_data.settings, setting)
end

local zwave = require( "zwave" )
local cc_wake_up = zwave.get_command_class(inclusion_context.zwave_node.id, 0, class_ids.wake_up)

if not cc_wake_up and build_tree_context.command_class.version and build_tree_context.command_class.version >= 3 then
    add_setting("identify")
end
