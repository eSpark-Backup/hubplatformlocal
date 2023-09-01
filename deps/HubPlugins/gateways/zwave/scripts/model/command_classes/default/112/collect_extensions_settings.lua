local self, device_node, create_map = ...

local result_map = {}
local result = {}

local extensions_manager = require "extensions_manager"
if not extensions_manager then 
    print("Extensions manager functionality not exists")
    return result
end

if not extensions_manager.get_plugin_extensions or not extensions_manager.get_plugin_extension_data then
    print("Extensions api not supported")
    return result
end

local function has_needed_fields(setting_info)
    return setting_info.configuration 
       and setting_info.configuration.number 
       and setting_info.name
       and setting_info.description
       and setting_info.type
end

local extensions = extensions_manager.get_plugin_extensions("zwave")
if extensions then
    local dev_handler_id = device_node.data.info.device_type_id

    for _, extension in ipairs(extensions) do
        local settings_count = extensions_manager.get_plugin_extension_data("zwave", extension .. "/deviceSettings/" .. dev_handler_id, "count")
        for i = 0, settings_count - 1 do
            local setting_info = extensions_manager.get_plugin_extension_data("zwave", extension .. "/deviceSettings/" .. dev_handler_id, tostring(i))
            if not result_map[setting_info.configuration.number] and has_needed_fields(setting_info) then
                local path = {path = extension .. "/deviceSettings/" .. dev_handler_id, index = i}
                result_map[setting_info.configuration.number] = path
                table.insert(result, path)
            end
            hubplugins.try_collectgarbage()
        end
        hubplugins.try_collectgarbage()
    end
end

return create_map and result_map or result
