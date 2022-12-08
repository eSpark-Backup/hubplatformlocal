local params = ...

local core = require "core"

params.gateway_id = core.get_gateway().id

print( "TEST_PLUGIN: Add device" )
core.add_device( params )
