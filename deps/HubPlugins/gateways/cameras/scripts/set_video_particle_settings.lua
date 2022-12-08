local params = ...

local cameras = require "cameras"
if not cameras then
    print( "Fail to load the cameras functionality" )
    return 
end

cameras.set_video_particle_settings( params.preRecordDurationSec,
                                     params.recordDurationSec,
                                     params.resolutionDivisor,
                                     params.format )
