local self = ...

local core = assert( require( "core" ), "Fail to load the 'core' functionality" )
local storage = hubplugins.object( "storage_helper", "HUB:zwave/scripts/utils" )

local core_subscriber_id = core.subscribe( "HUB:zwave/scripts/events/events_handling", { rules = {} } ) -- block all events
storage:call( "set_string", "core_subscriber_id", core_subscriber_id )
