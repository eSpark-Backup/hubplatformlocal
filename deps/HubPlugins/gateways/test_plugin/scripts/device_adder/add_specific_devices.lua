local device_type, devices_count = ...

for i = 1, devices_count do
    loadfile( "HUB:test_plugin/scripts/device_adder/add_device" )( device_type )
end
