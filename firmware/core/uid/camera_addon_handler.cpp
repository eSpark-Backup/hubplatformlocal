#include "uid/camera_addon_handler.h"
#include "json_utils/json_utils.h"
#include "jsonrpc/jsonrpc.h"
#include "base_utils/utils.h"
#include "system_utils/filesystem.h"
#include "common_utils/cpp14.h"
#include "http_bridge/session.h"
#include "http_bridge/ws_api_bridge.h"

namespace HubPlatform {
namespace Uid {

const int CamerasEndpointHandler::CameraAddonRequestTimeout;

CamerasEndpointHandler::CamerasEndpointHandler( Services::Service& hostService )
: m_hostService( hostService ) {}

void CamerasEndpointHandler::getSnapshot( HttpBridge::CallbackParams params, const std::string& streamId ) {
    auto callback = [this, params, streamId]( const Json& message ) {
        auto filePath = json_get_str( message, "filePath" );
        utils::AfterReturn afterReturn( [this, params, filePath, streamId]() {
            if( isSessionClosed( params.m_wsi ) ) {
                if( !filePath.empty() ) {
                    Log::trace( "remove snapshot for stream " + cc::attention( streamId ) );
                    filesystem::removeFile( filePath.c_str() );
                }
                return;
            }
            callbackOnWritable( params );
        } );
        std::lock_guard< std::mutex > lock( m_mutex );
        Log::trace( "response from camera addon for stream " + cc::attention( streamId ) + ": " + cc::j( message ) );
        if( hasError( message ) ) {
            std::string error = json_get_str( message[ "error" ], "message" );
            Log::error( cc::error( error + ": " ) + cc::attention( streamId ) );
            m_files[ params.m_user ].error = error;
            return;
        }
        if( filePath.empty() ) {
            const std::string& error = "empty file from camera addon";
            Log::error( cc::error( error + ": " ) + cc::attention( streamId ) );
            m_files[ params.m_user ].error = error;
            return;
        }
        auto file = File( fopen( filePath.c_str(), "rb" ), []( FILE* file ) {
            if( file )
                fclose( file );
        } );
        if( file == nullptr ) {
            const std::string& error = "can not open file";
            Log::error( cc::error( error + ": " ) + cc::attention( streamId ) );
            m_files[ params.m_user ].error = error;
            return;
        }
        auto& state = m_files[ params.m_user ];
        state.file = std::move( file );
        state.error = "";
        state.fileSize = filesystem::getFileSize( filePath.c_str() );
        state.fileAutoRemover = FileAutoRemover{ filePath };
    };
    auto timeoutCallback = [this, params, streamId]() {
        if( isSessionClosed( params.m_wsi ) ) {
            return;
        }
        const std::string& error = "no response from camera addon";
        Log::error( cc::error( error + ": " ) + cc::attention( streamId ) );
        std::lock_guard< std::mutex > lock( m_mutex );
        m_files[ params.m_user ].error = error;
        callbackOnWritable( params );
    };
    m_hostService.sendAsyncRequest( "addon.cameras",
                                    JSON( "streamId" << streamId ),
                                    CAMERAS_MAKE_SNAPSHOT_FOR_UI,
                                    callback,
                                    timeoutCallback,
                                    std::chrono::seconds( CameraAddonRequestTimeout ) );
}

bool CamerasEndpointHandler::isHandlerValid() {
    return true;
}

bool CamerasEndpointHandler::isSessionClosed( lws* wsi ) const {
    HttpBridge::SessionPtr pss = HttpBridge::getApiBridge().findSession( wsi );
    if( !pss || !pss->getData() ) {
        Log::error( cc::error( "Http session is not found for " ) + cc::num0x( reinterpret_cast< uint64_t >( wsi ) ) );
        return true;
    }
    auto& sessionData = *pss->getData();
    if( sessionData.isFinished() ) {
        Log::warn( cc::warn( "Http session is closed" ) );
        return true;
    }
    return false;
}

}  // namespace Uid
}  // namespace HubPlatform
