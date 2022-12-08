print("water_valve: teardown")

local water_valve = assert( require "water_valve", "Fail to load the water_valve functionality" )

water_valve.unsubscribe( "HUB:water_valve/scripts/events_handling" )
