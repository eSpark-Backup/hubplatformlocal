local params = ...

local cameras = require "cameras"
if not cameras then
    print( "Fail to load the cameras functionality" )
    return 
end

cameras.create_live_rtsp_stream( params.streamId )
