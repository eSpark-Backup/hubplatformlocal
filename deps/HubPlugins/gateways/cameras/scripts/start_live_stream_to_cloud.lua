local params = ...

local core = require "core"
if not core then
    print( "Fail to load the core functionality" )
    return
end

local cameras = require "cameras"
if not cameras then
    print( "Fail to load the cameras functionality" )
    return 
end

cameras.start_live_stream_to_cloud( params.streamId )
