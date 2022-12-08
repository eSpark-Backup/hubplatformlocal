local self, device_info, node_id = ... --table is passed by reference

local zwave = assert( require( "zwave" ), "Fail to load the 'zwave' functionality" )

--use information a user provided by a step before the inclusion step
local smart_start_info = assert( zwave.get_provisioning_info( node_id ), "Failed to get provisioning SmartStart info for node" )
if string.len( smart_start_info.name ) ~= 0 then
    device_info.name = smart_start_info.name
end
if string.len( smart_start_info.room ) ~= 0 then
    device_info.roomId = smart_start_info.room
end
