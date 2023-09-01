local cameras = require "cameras"
if not cameras then
    print( "Fail to load the cameras functionality" )
    return 
end

cameras.subscribe( "HUB:cameras/scripts/events_handling" )
