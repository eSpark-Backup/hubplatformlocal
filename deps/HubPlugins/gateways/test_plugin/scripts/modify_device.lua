local params = ...

local core = require "core"

params.device.gateway_id = core.get_gateway().id

print( "TEST_PLUGIN: Modify device" )
core.modify_device( params.device_id, params.device )
