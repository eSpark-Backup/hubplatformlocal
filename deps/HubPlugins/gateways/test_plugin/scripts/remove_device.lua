local params = ...

local core = require "core"

local root_device_id = core.get_root_device_id( params.device_id )

if root_device_id ~= params.device_id then
    print( string.format( "TEST_PLUGIN: Remove device, device_id: %s (%s)", params.device_id, root_device_id ) )
else
    print( "TEST_PLUGIN: Remove device, device_id: " .. params.device_id )
end

core.remove_device( root_device_id )
