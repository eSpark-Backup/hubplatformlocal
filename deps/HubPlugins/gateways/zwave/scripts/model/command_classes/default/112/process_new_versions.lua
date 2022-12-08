local self, inclusion_context, build_tree_context, device_node = ...

local cc = build_tree_context.command_class
local dev_handler_id = device_node.data.info.device_type_id
local zwave_node_id = inclusion_context.zwave_node.id

if cc.parameters then
    local device_specific_props_map = self:call("collect_device_specific_settings", device_node)
    local extensions_props_map = self:call("collect_extensions_settings", device_node, true)
    
    for _, parameter in ipairs(cc.parameters) do
        local setting_info
        
        if extensions_props_map[parameter.id] ~= nil then
            local extensions_manager = require "extensions_manager"
            setting_info = extensions_manager.get_plugin_extension_data(
                "zwave", 
                extensions_props_map[parameter.id].path, 
                tostring(extensions_props_map[parameter.id].index)
            )
        elseif device_specific_props_map[parameter.id] ~= nil then
            setting_info = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/get_device_setting_info")(dev_handler_id, device_specific_props_map[parameter.id])
        else
            setting_info = self:call("get_setting_info_from_zwave", parameter)
        end
        if self:call("should_add_setting", setting_info) then
            local custom_setting = self:call("create_setting", zwave_node_id, dev_handler_id, setting_info)
            table.insert(device_node.data.settings, custom_setting)
        end
    end
end
