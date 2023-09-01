local item_names, device_id = ...

local core = require "core"

for _, item_name in ipairs( item_names ) do
    local item_info = loadfile( "HUB:test_plugin/scripts/item_info/" .. item_name )
    if not item_info then
        print( "TEST_PLUGIN: Unsupported item name: " .. item_name )
        return
    end  
    item_info = item_info()
    
    item_info.device_id = device_id
    core.add_item(item_info)
end
