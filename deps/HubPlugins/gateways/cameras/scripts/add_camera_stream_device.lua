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


local gateway = core.get_gateway()
local dev_id = core.add_device( { gateway_id = gateway.id,
                                  name = params.streamName,
                                  type = "camera",
                                  device_type_id = params.streamName,
                                  category = "camera",
                                  battery_powered = false } )
if dev_id then
    local value = { localRtspUrl = "" }
    local item = { device_id = dev_id,
                   name = "camera_stream",
                   value_type = "cameraStream",
                   has_getter = true,
                   has_setter = false,
                   value = value }
    local item_id = core.add_item( item )
    if item_id then
        print( "Created camera item with id ", item_id )
        -- Using item_id as streamId
        local success = cameras.add_camera_stream( item_id, params.streamName, params.rtspUrl, params.description )
    else
        print( "Error occurred while adding camera stream item" )
        core.remove_device( dev_id )
    end
else
    print( "Error occurred while adding camera stream device" )
end
