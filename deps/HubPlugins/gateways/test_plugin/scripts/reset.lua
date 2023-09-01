local core = require "core"
local storage = require "storage"

print( "TEST_PLUGIN: Reset" )
core.remove_gateway_devices( core.get_gateway().id )
storage.delete_all()
