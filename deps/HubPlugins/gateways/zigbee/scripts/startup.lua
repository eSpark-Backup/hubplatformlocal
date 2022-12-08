print("LUA: Zigbee startup...")

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert(require "zigbee", "Fail to load zigbee functionality")
local house_modes = assert(require "house_modes", "Fail to load house_modes functionality")

print("LUA: Subscribe on event handling...")
zigbee.subscribe( "HUB:zigbee/scripts/events/zigbee/events_handling" )
house_modes.subscribe( "HUB:zigbee/scripts/events/house_modes/events_handling" )

hubplugins.weak_loadfile("HUB:zigbee/scripts/helpers/request_all_device_values")()
