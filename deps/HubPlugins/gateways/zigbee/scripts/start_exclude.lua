local params = ...

print("LUA: Try start exclude...")

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert( require "zigbee", "Fail to load the zigbee functionality" )

if params and params.nodeId then
    zigbee.start_exclude(params.nodeId)
else
    zigbee.start_exclude()
end
