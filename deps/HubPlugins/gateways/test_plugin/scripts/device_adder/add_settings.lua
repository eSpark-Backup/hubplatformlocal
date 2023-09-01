local setting_names, device_id = ...

local core = require "core"

for _, setting_name in ipairs( setting_names ) do
    local setting_info = loadfile( "HUB:test_plugin/scripts/setting_info/" .. setting_name )
    if not setting_info then
        print( "TEST_PLUGIN: Unsupported item name: " .. setting_name )
        return
    end  
    setting_info = setting_info()
    
    setting_info.device_id = device_id
    core.add_setting(setting_info)
end
