local zwave = require "zwave"
if not zwave then
    print( "Fail to load the zwave functionality" )
    return
end

zwave.stop_learn()
