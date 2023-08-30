print("teardown")

local plughub = assert( require "plughub", "Fail to load the plughub functionality" )

plughub.unsubscribe( "HUB:plughub/scripts/events_handling" )
