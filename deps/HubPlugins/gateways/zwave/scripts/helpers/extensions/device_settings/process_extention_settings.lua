local extensions_manager, extensions_with_pri = ...

local processed_devices = {}

for _, extension in ipairs(extensions_with_pri) do
    local device_type_ids = extensions_manager.get_plugin_extension_data("zwave", extension .. "/deviceSettings", "keys")
    if device_type_ids then 
        for _, device_type_id in ipairs(device_type_ids) do
            if not processed_devices[device_type_id] then
                processed_devices[device_type_id] = true
                hubplugins.weak_loadfile("HUB:zwave/scripts/helpers/extensions/device_settings/process_device_settings")(
                    extensions_manager, 
                    extensions_with_pri, 
                    device_type_id 
                )
            end
        end
    end
end
