local params = ...

local cameras = require "cameras"
if not cameras then
    print( "Fail to load the cameras functionality" )
    return 
end

cameras.create_stream_saving_to_cloud( params.streamId, params.segmentDurationSec, params.secondsBeforeStart )

