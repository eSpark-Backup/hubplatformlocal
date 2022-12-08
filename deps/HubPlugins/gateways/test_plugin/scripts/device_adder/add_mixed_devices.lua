local devices_count = ...

local infos_set = loadfile( "HUB:test_plugin/scripts/device_info/infos_set" )()

local counter = 0

while true do
    for _, device_type in ipairs(infos_set) do
        counter = counter + 1
        if counter > devices_count then return end
        
        loadfile( "HUB:test_plugin/scripts/device_adder/add_device" )( device_type )
    end
end
