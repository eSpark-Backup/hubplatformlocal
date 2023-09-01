print("LUA: teardown")

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert( require "zigbee", "Fail to load the zigbee functionality" )
local house_modes = assert(require "house_modes", "Fail to load house_modes functionality")

zigbee.unsubscribe( "HUB:zigbee/scripts/events/zigbee/events_handling" )
house_modes.unsubscribe( "HUB:zigbee/scripts/events/house_modes/events_handling" )
