local params = ...

local cameras = require "cameras"
if not cameras then
    print( "Fail to load the cameras functionality" )
    return 
end

if params.metaData then
    cameras.make_snapshot( params.streamId, params.snapshotId, params.metaData  )
else
    cameras.make_snapshot( params.streamId, params.snapshotId )
end
