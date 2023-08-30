local params = ...

local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

zwave.set_device_specific_key( params.key )
