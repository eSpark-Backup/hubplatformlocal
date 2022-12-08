local extensions_manager, extensions_with_pri, device_type_id = ...

local setting_checker = hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/extensions/device_settings/check_setting_format")

local processed_settings = {}

for _, extension in ipairs(extensions_with_pri) do
    local settings_count = extensions_manager.get_plugin_extension_data("zwave", extension .. "/deviceSettings/" .. device_type_id, "count")
    for i = 0, settings_count - 1 do
        local setting_info = extensions_manager.get_plugin_extension_data("zwave", extension .. "/deviceSettings/" .. device_type_id, tostring(i))
        if setting_checker(setting_info) and not processed_settings[setting_info.configuration.number] then
            processed_settings[setting_info.configuration.number] = true
            hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/extensions/device_settings/process_device_setting")(device_type_id, setting_info)
        end
    end
end

processed_settings = {}
