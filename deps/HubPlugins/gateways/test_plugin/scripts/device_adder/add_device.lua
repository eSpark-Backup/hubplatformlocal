local device_type = ...

local core = require "core"
local add_items = loadfile( "HUB:test_plugin/scripts/device_adder/add_items" )
local add_settings = loadfile( "HUB:test_plugin/scripts/device_adder/add_settings" )


-- main


local device_info = loadfile( "HUB:test_plugin/scripts/device_info/" .. device_type )
if not device_info then
    print( "TEST_PLUGIN: Unsupported device type: " .. device_type )
    return
end

device_info = device_info()

local gateway_id = core.get_gateway().id

device_info.master_device.gateway_id = gateway_id
device_info.master_device.name = loadfile( "HUB:test_plugin/scripts/device_adder/get_unique_name" )( device_info.master_device.name )
local master_device_id = core.add_device( device_info.master_device )
add_items(device_info.master_device.items, master_device_id)

if device_info.master_device.settings then
    add_settings(device_info.master_device.settings, master_device_id)
end

for _, child_device in ipairs( device_info.child_devices ) do
    child_device.gateway_id = gateway_id
    child_device.parent_device_id = master_device_id
    child_device.name = loadfile( "HUB:test_plugin/scripts/device_adder/get_unique_name" )( child_device.name )
    
    local child_device_id = core.add_device( child_device )
    add_items( child_device.items, child_device_id )
    
    if child_device.settings then
        add_settings(child_device.settings, child_device_id)
    end
end
