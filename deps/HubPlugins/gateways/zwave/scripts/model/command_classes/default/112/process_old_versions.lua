local self, inclusion_context, build_tree_context, device_node = ...

local zwave = require("zwave")

local zwave_node_id = inclusion_context.zwave_node.id

do
    local zwave_configurations = hubplugins.weak_loadfile("HUB:zwave/scripts/model/settings/zwave_configurations")(zwave_node_id)
    table.insert(device_node.data.settings, zwave_configurations)
end

local dev_handler_id = device_node.data.info.device_type_id

local function add_setting(setting_info)
    local custom_setting = self:call("create_setting", zwave_node_id, dev_handler_id, setting_info)

    table.insert(device_node.data.settings, custom_setting)

    zwave.add_configuration(zwave_node_id, setting_info.configuration.number, setting_info.configuration.size)

    setting_info = nil
    hubplugins.try_collectgarbage()
end

local added_props = {}
local extensions_props = self:call("collect_extensions_settings", device_node)
for _, setting_path in ipairs(extensions_props) do
    local extensions_manager = require "extensions_manager"
    local setting_info = extensions_manager.get_plugin_extension_data(
        "zwave", 
        setting_path.path, 
        tostring(setting_path.index)
    )
    added_props[setting_info.configuration.number] = true
    add_setting(setting_info)
end

local settings_count = device_node.data.info.settings_count or 0

for idx = 1, settings_count do
    local setting_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_setting_info")(dev_handler_id, idx)
    if not added_props[setting_info.configuration.number] then
        add_setting(setting_info)
    end
end
