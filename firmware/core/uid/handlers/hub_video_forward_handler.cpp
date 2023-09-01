#include "uid/handlers/hub_video_forward_handler.h"
#include "service_interfaces/service.h"
#include "uid/ui_service.h"

namespace HubPlatform {
namespace Uid {

HubVideoForwardHandler::HubVideoForwardHandler( Uid::Service& hostService )
: m_service( hostService ) {}

void HubVideoForwardHandler::handleVideoForward( rpc::Message& rpcMessage ) {
    try {
        const JsonObject& joParams = rpcMessage.getParams();
        auto& joAnswer = rpcMessage.getAnswer();
        auto peer = rpcMessage.getNetworkObject();

        auto callback = [peer, joAnswer]( const JsonObject& response ) mutable {
            setErrorIfHas( joAnswer, response );
            joAnswer[ "result" ] = std::move( response );
            json_remove( joAnswer[ "result" ], Errors::ERROR_FIELD );

            if( peer ) {
                peer->sendMessage( joAnswer );
            }
        };

        m_service.sendAsyncRequest( GroupsNames::WEBRTC,
                                    joParams,
                                    MessageSubClasses::HUB_VIDEOFORWARD,
                                    std::move( callback ),
                                    std::chrono::minutes( 2 ) );
        joAnswer = nullptr;
    }
    catch( const std::exception& e ) {
        setError( rpcMessage.getAnswer(), Errors::WRONG_FIELD, e.what() );
    }
}

}  // namespace Uid
}  // namespace HubPlatform
