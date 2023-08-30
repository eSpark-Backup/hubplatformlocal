local params = ...

require ("HUB:zigbee/scripts/helpers/scripts_preload")

local zigbee = assert( require "zigbee", "Fail to load zigbee functionality" )
assert( params.nodeId, "Fail to set value: nodeId required" )
assert( params.classId, "Fail to set value: classId required" )
assert( params.channelId, "Fail to set value: channelId required" )
assert( params.value, "Fail to set value: value required" )

zigbee.set_value( params.nodeId, params.classId, params.channelId, params.value)


