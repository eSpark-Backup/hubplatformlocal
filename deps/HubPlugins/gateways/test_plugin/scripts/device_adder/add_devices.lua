local params = ...

local device_type = params.device_type
local devices_count = params.devices_count

if device_type then
    loadfile( "HUB:test_plugin/scripts/device_adder/add_specific_devices" )( device_type, devices_count )
else
    loadfile( "HUB:test_plugin/scripts/device_adder/add_mixed_devices" )( devices_count )
end
