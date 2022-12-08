local params = ...

local core = require "core"
if not core then
    print( "Fail to load the core functionality" )
    return
end


local event = params.event

if event == "live_rtsp_stream_started" then
    core.update_item_value( params.streamId, { localRtspUrl = params.localRtspUrl } )
elseif event == "live_rtsp_stream_stopped" then
    core.update_item_value( params.streamId, { localRtspUrl = "" } )
elseif event == "live_rtsp_stream_error" then
    print( params.errorMessage )
    core.send_ui_broadcast({ event = event, errorMessage = params.errorMessage })
elseif event == "stream_snapshotted" then
    print( params.errorMessage )
    core.send_ui_broadcast({ event = event, snapshotId = params.snapshotId, cloudStorageKey = params.cloudStorageKey })
elseif event == "stream_snapshot_error" then
    print( params.errorMessage )
    core.send_ui_broadcast({ event = event, snapshotId = params.snapshotId, errorMessage = params.errorMessage })
end
