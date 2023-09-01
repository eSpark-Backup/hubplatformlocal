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

cameras.remove_camera_stream( params.streamId )
local item = core.get_item( params.streamId )
if item then
    core.remove_device( item.device_id )
end
