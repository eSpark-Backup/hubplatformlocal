local params = ...

print( "Core device updated" )

local zwave = assert( require( "zwave" ), "Fail to load the 'zwave' functionality" )
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )

local device_node_id = storage:call( "find_node_id", params._id )

zwave.set_provisioning_info( device_node_id, params.name, params.roomId )
